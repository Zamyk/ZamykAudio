#include <ZAudio/PhaserEffect.h>

namespace ZAudio {


PhaserEffect::PhaserEffect(Parameters parameters_p) : parameters(parameters_p) {}

void PhaserEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = phaseShifter.process(in[0]) * parameters.wet.linear() + in[0] * parameters.dry.linear();
}

void PhaserEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case RateID:
      parameters.rate = value.getFrequency();
      phaseShifter.setRate(parameters.rate);
      break;
    case DepthID:
      parameters.depth = value.getNonInteger();
      phaseShifter.setDepth(parameters.depth);
      break;
    case WetID:
      parameters.wet = value.getVolume();
      break;
    case DryID:
      parameters.dry = value.getVolume();      
      break;
    default:
      assert(false);
  }
}

uint32_t PhaserEffect::getTailTime() const {
  return 0;
}

void PhaserEffect::setSampleRate(Frequency sampleRate) {
  phaseShifter = Tools::PhaseShifter(sampleRate, parameters.depth, parameters.rate, 0.);
}

std::unique_ptr<Effect> PhaserEffect::clone() const {
  return std::make_unique<PhaserEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, PhaserEffect::NumOfParameters> names = {
    "Rate",
    "Depth",
    "Wet",
    "Dry"
  };
  return names[ind];
}  

Result PhaserEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue(getName(RateID), parameters.rate);  
  result &= writer.addValue(getName(DepthID), parameters.depth);  
  result &= writer.addValue(getName(WetID), parameters.wet);  
  result &= writer.addValue(getName(DryID), parameters.dry);    
  return result;
}

Result PhaserEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(RateID), parameters.rate);  
  result &= reader.getValue(getName(DepthID), parameters.depth);  
  result &= reader.getValue(getName(WetID), parameters.wet);  
  result &= reader.getValue(getName(DryID), parameters.dry);  
  return result;
}

std::string PhaserEffect::getID() const {
  return "ZA_PhaserEffect";
}

int64_t PhaserEffect::getVersion() const {
  return 1;
}

PhaserEffect::Parameters PhaserEffect::getParameters() const {
  return parameters;
}


} // namespace ZAudio