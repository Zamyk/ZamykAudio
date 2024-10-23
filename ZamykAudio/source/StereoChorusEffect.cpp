#include <ZAudio/StereoChorusEffect.h>

namespace ZAudio {


StereoChorusEffect::Parameters::Parameters(Frequency rate_p, Time minDelay_p, Time maxDelay_p) : 
  rate(rate_p),
  minDelay(minDelay_p),
  maxDelay(maxDelay_p) {}

StereoChorusEffect::StereoChorusEffect(Parameters parameters_p) : parameters(parameters_p) {}

void StereoChorusEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  double y1 = leftModDelay.process(in[0]);
  double y2 = rightModDelay.process(in[1]);
  double y3 = middleModDelay.process(0.5 * y1 + 0.5 * y2);    
  out[0] = y1 * 0.5 + y3 * 0.5;  
  out[1] = y2 * 0.5 + y3 * 0.5;
}

void StereoChorusEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case RateID:
      parameters.rate = value.getFrequency();
      leftModDelay.setRate(getLeftDelayRate());
      middleModDelay.setRate(getMiddleDelayRate());
      rightModDelay.setRate(getRightDelayRate());
      break;
    case MinDelayID:
      parameters.minDelay = value.getTime();
      leftModDelay.setMinDelay(parameters.minDelay);
      middleModDelay.setMinDelay(parameters.minDelay);
      rightModDelay.setMinDelay(parameters.minDelay);
      break;
    case MaxDelayID:
      parameters.maxDelay = value.getTime();      
      leftModDelay.setMaxDelay(parameters.maxDelay);
      middleModDelay.setMaxDelay(parameters.maxDelay);
      rightModDelay.setMaxDelay(parameters.maxDelay);
      break;    
    default:
      assert(false);
  }
}

uint32_t StereoChorusEffect::getTailTime() const {
  return std::max({leftModDelay.getTailTime(), middleModDelay.getTailTime(), rightModDelay.getTailTime()});
}

Frequency StereoChorusEffect::getLeftDelayRate() const {
  return parameters.rate * 0.5;
}

Frequency StereoChorusEffect::getMiddleDelayRate() const {
  return parameters.rate;
}

Frequency StereoChorusEffect::getRightDelayRate() const {
  return parameters.rate * 0.75;
}

void StereoChorusEffect::setSampleRate(Frequency sampleRate) {
  Tools::ModulatedDelay::Parameters params;
  params.dry = Volume::dB(0);
  params.wet = Volume::dB(-3);
  params.feedback = Volume::linear(0);
  params.minDelay = parameters.minDelay;
  params.reservedDelay = parameters.maxDelay;
  params.maxDelay = parameters.maxDelay;
  params.rate = getMiddleDelayRate();
  params.shape = Tools::LowFrequencyOscillator::ShapeType::Triangle;    

  middleModDelay = Tools::ModulatedDelay(sampleRate, params);
  
  params.phaseOffset = 0.25;  
  params.rate = getLeftDelayRate();
  leftModDelay = Tools::ModulatedDelay(sampleRate, params);  

  params.phaseOffset = 0.75;  
  params.rate = getRightDelayRate();
  rightModDelay = Tools::ModulatedDelay(sampleRate, params);  
}

std::unique_ptr<Effect> StereoChorusEffect::clone() const {
  return std::make_unique<StereoChorusEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, StereoChorusEffect::NumOfParameters> names = {
    "Rate",
    "MinDelay",
    "MaxDelay"
  };
  return names[ind];
}  

Result StereoChorusEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addValue(getName(RateID), parameters.rate);
  result &= writer.addValue(getName(MinDelayID), parameters.minDelay);
  result &= writer.addValue(getName(MaxDelayID), parameters.maxDelay);  
  return result;
}

Result StereoChorusEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(RateID), parameters.rate);
  result &= reader.getValue(getName(MinDelayID), parameters.minDelay);
  result &= reader.getValue(getName(MaxDelayID), parameters.maxDelay);  
  return result;
}

std::string StereoChorusEffect::getID() const {
  return "ZA_StereoChorusEffect";
}

int64_t StereoChorusEffect::getVersion() const {
  return 2;
}

StereoChorusEffect::Parameters StereoChorusEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio