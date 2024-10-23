#include <ZAudio/FlangerEffect.h>

namespace ZAudio {


FlangerEffect::Parameters::Parameters(Frequency rate_p, Time minDelay_p, Time maxDelay_p) : 
  rate(rate_p),
  minDelay(minDelay_p),
  maxDelay(maxDelay_p) {}

FlangerEffect::Parameters::Parameters(Frequency rate_p, Time minDelay_p, Time maxDelay_p, Volume feedback_p) : 
  rate(rate_p),
  minDelay(minDelay_p),
  maxDelay(maxDelay_p),
  feedback(feedback_p) {}

FlangerEffect::FlangerEffect(Parameters parameters_p) : parameters(parameters_p) {}  

void FlangerEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = modDelay.process(in[0]);
}

void FlangerEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case RateID:
      parameters.rate = value.getFrequency();    
      modDelay.setRate(parameters.rate);
      break;

    case MinDelayID:
      parameters.minDelay = value.getTime();
      modDelay.setMaxDelay(parameters.minDelay);
      break;
    
    case MaxDelayID:
      parameters.maxDelay = value.getTime();
      modDelay.setMaxDelay(parameters.maxDelay);
      break;
    
    case FeedbackID:
      parameters.feedback = value.getVolume();
      modDelay.setFeedback(parameters.feedback);
      break;
    default:
      assert(false);
  }
}

uint32_t FlangerEffect::getTailTime() const {
  return modDelay.getTailTime();
}

void FlangerEffect::setSampleRate(Frequency sampleRate) {
  Tools::ModulatedDelay::Parameters params;
  params.dry = Volume::linear(0.5);
  params.wet = Volume::linear(0.5);
  params.feedback = parameters.feedback;
  params.minDelay = parameters.minDelay;
  params.reservedDelay = parameters.maxDelay;
  params.maxDelay = parameters.maxDelay;  
  params.rate = parameters.rate;
  params.shape = Tools::LowFrequencyOscillator::ShapeType::Sine;    
  modDelay = Tools::ModulatedDelay(sampleRate, params);
}

std::unique_ptr<Effect> FlangerEffect::clone() const {
  return std::make_unique<FlangerEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, FlangerEffect::NumOfParameters> names = {
    "Rate",
    "MinDelay",
    "MaxDelay",
    "Feedback"    
  };
  return names[ind];
}  

Result FlangerEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue(getName(RateID), parameters.rate);  
  result &= writer.addValue(getName(MinDelayID), parameters.minDelay);  
  result &= writer.addValue(getName(MaxDelayID), parameters.maxDelay);  
  result &= writer.addValue(getName(FeedbackID), parameters.feedback);    
  return result;
}

Result FlangerEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();
  result &= reader.getValue(getName(RateID), parameters.rate);
  result &= reader.getValue(getName(MinDelayID), parameters.minDelay);
  result &= reader.getValue(getName(MaxDelayID), parameters.maxDelay);
  result &= reader.getValue(getName(FeedbackID), parameters.feedback);  
  return result;
}

std::string FlangerEffect::getID() const {
  return "ZA_FlangerEffect";
}

int64_t FlangerEffect::getVersion() const {
  return 2;
}

FlangerEffect::Parameters FlangerEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio