#include <ZAudio/StereoFlangerEffect.h>

namespace ZAudio {


StereoFlangerEffect::Parameters::Parameters(Frequency rate_p, Time minDelay_p, Time maxDelay_p, Volume feedback_p) : 
  rate(rate_p),
  minDelay(minDelay_p),
  maxDelay(maxDelay_p),
  feedback(feedback_p) {}

StereoFlangerEffect::StereoFlangerEffect(Parameters parameters_p) : parameters(parameters_p) {}

void StereoFlangerEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = leftModDelay.process(in[0]);
  out[1] = rightModDelay.process(in[1]);
}

void StereoFlangerEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case RateID:
      parameters.rate = value.getFrequency();
      leftModDelay.setRate(parameters.rate);
      rightModDelay.setRate(parameters.rate);
      break;
    case MinDelayID:      
      parameters.minDelay = value.getTime();      
      leftModDelay.setMinDelay(parameters.minDelay);
      rightModDelay.setMinDelay(parameters.minDelay);
      break;
    case MaxDelayID:
      parameters.maxDelay = value.getTime();
      leftModDelay.setMaxDelay(parameters.maxDelay);
      rightModDelay.setMaxDelay(parameters.maxDelay);
      break;
    case FeedbackID:
      parameters.feedback = value.getVolume();
      leftModDelay.setFeedback(parameters.feedback);
      rightModDelay.setFeedback(parameters.feedback);
      break;    
    default:
      assert(false);
  }
}

uint32_t StereoFlangerEffect::getTailTime() const {
  return std::max(leftModDelay.getTailTime(), rightModDelay.getTailTime());
}
void StereoFlangerEffect::setSampleRate(Frequency sampleRate) {
  Tools::ModulatedDelay::Parameters params;
  params.dry = Volume::linear(0.5);
  params.wet = Volume::linear(0.5);
  params.feedback = parameters.feedback;
  params.minDelay = parameters.minDelay;
  params.reservedDelay = parameters.maxDelay;
  params.maxDelay = parameters.maxDelay;
  params.rate = parameters.rate;  
  params.shape = Tools::LowFrequencyOscillator::ShapeType::Triangle;  
  leftModDelay = Tools::ModulatedDelay(sampleRate, params);  
  params.phaseOffset = 0.25;  
  rightModDelay = Tools::ModulatedDelay(sampleRate, params);
}

std::unique_ptr<Effect> StereoFlangerEffect::clone() const {
  return std::make_unique<StereoFlangerEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, StereoFlangerEffect::NumOfParameters> names = {
    "Rate",
    "MinDelay",
    "MaxDelay",
    "Feedback"
  };
  return names[ind];
}  

Result StereoFlangerEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue(getName(RateID), parameters.rate);
  result &= writer.addValue(getName(MinDelayID), parameters.minDelay);
  result &= writer.addValue(getName(MaxDelayID), parameters.maxDelay);
  result &= writer.addValue(getName(FeedbackID), parameters.feedback);
  return result;
}

Result StereoFlangerEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(RateID), parameters.rate);
  result &= reader.getValue(getName(MinDelayID), parameters.minDelay);
  result &= reader.getValue(getName(MaxDelayID), parameters.maxDelay);
  result &= reader.getValue(getName(FeedbackID), parameters.feedback);
  return result;
}

std::string StereoFlangerEffect::getID() const {
  return "ZA_StereoFlangerEffect";
}

int64_t StereoFlangerEffect::getVersion() const {
  return 2;
}

StereoFlangerEffect::Parameters StereoFlangerEffect::getParameters() const {
  return parameters;
}


} // namespace ZAudio