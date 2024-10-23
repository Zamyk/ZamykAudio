#include <ZAudio/VibratoEffect.h>

namespace ZAudio {


VibratoEffect::VibratoEffect(Parameters parameters_p) : parameters(parameters_p) {}

void VibratoEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = modDelay.process(in[0]);
}

void VibratoEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case RateID:
      parameters.rate = value.getFrequency();
      modDelay.setRate(parameters.rate);
      break;
    case MinDelayID:
      parameters.minDelay = value.getTime();
      modDelay.setMinDelay(parameters.minDelay);
      break;
    case MaxDelayID:
      parameters.maxDelay = value.getTime();
      modDelay.setMaxDelay(calculateMaxDelay());
      break;
    case DepthID:
      parameters.depth = value.getNonInteger();
      modDelay.setMaxDelay(calculateMaxDelay());
      break;
    default:
      assert(false);
  }
}

Time VibratoEffect::calculateMaxDelay() const {
  return Time::seconds(NormalizedValue(parameters.depth).bind(parameters.minDelay.seconds(), parameters.maxDelay.seconds()));
}

uint32_t VibratoEffect::getTailTime() const {
  return modDelay.getTailTime();
}

void VibratoEffect::setSampleRate(Frequency sampleRate) {
  Tools::ModulatedDelay::Parameters params;
  params.dry = Volume::linear(0.);
  params.wet = Volume::linear(1.);
  params.feedback = Volume::linear(0.);
  params.minDelay = parameters.minDelay;
  params.reservedDelay = parameters.maxDelay;
  params.maxDelay = calculateMaxDelay();
  params.rate = parameters.rate;
  params.shape = Tools::LowFrequencyOscillator::ShapeType::Sine;  
  modDelay = Tools::ModulatedDelay(sampleRate, params);
}

std::unique_ptr<Effect> VibratoEffect::clone() const {
  return std::make_unique<VibratoEffect>(parameters);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, VibratoEffect::NumOfParameters> names = {
    "Rate",
    "MinDelay",
    "MaxDelay",
    "Depth"
  };
  return names[ind];
}  

Result VibratoEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addValue(getName(RateID), parameters.rate);
  result &= writer.addValue(getName(MinDelayID), parameters.minDelay);
  result &= writer.addValue(getName(MaxDelayID), parameters.maxDelay);
  result &= writer.addValue(getName(DepthID), parameters.depth);
  return result;
}

Result VibratoEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(RateID), parameters.rate);
  result &= reader.getValue(getName(MinDelayID), parameters.minDelay);
  result &= reader.getValue(getName(MaxDelayID), parameters.maxDelay);
  result &= reader.getValue(getName(DepthID), parameters.depth);
  return result;
}

std::string VibratoEffect::getID() const {
  return "ZA_VibratoEffect";
}

int64_t VibratoEffect::getVersion() const {
  return 1;
}

VibratoEffect::Parameters VibratoEffect::getParameters() const {
  return parameters;
}


} // namespace ZAudio