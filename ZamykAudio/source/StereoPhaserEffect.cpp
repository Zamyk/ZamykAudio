#include <ZAudio/StereoPhaserEffect.h>

namespace ZAudio {


StereoPhaserEffect::Parameters::Parameters(Frequency rate_p, double depth_p, Volume wet_p, Volume dry_p) : 
  rate(rate_p),
  depth(depth_p),
  wet(wet_p),
  dry(dry_p) {}

StereoPhaserEffect::StereoPhaserEffect(Parameters parameters_p) : parameters(parameters_p) {}
  
void StereoPhaserEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  sample_t mixed = in[0] + in[1];
  out[0] = leftPhaseShifter.process(mixed) * parameters.wet.linear() + in[0] * parameters.dry.linear();
  out[1] = rightPhaseShifter.process(mixed) * parameters.wet.linear() + in[1] * parameters.dry.linear();
}

void StereoPhaserEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case RateID:
      parameters.rate = value.getFrequency();
      leftPhaseShifter.setRate(parameters.rate);
      rightPhaseShifter.setRate(parameters.rate);
      break;
    case DepthID:
      parameters.depth = value.getNonInteger();
      leftPhaseShifter.setDepth(parameters.depth);
      rightPhaseShifter.setDepth(parameters.depth);
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

uint32_t StereoPhaserEffect::getTailTime() const {
  return 0;
}

void StereoPhaserEffect::setSampleRate(Frequency sampleRate) {
  leftPhaseShifter = Tools::PhaseShifter(sampleRate, parameters.depth, parameters.rate, 0.);
  rightPhaseShifter = Tools::PhaseShifter(sampleRate, parameters.depth, parameters.rate, 0.25);
}

std::unique_ptr<Effect> StereoPhaserEffect::clone() const {
  return std::make_unique<StereoPhaserEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, StereoPhaserEffect::NumOfParameters> names = {
    "Rate",
    "Depth",
    "Wet",
    "Dry"
  };
  return names[ind];
}  

Result StereoPhaserEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue(getName(RateID), parameters.rate);  
  result &= writer.addValue(getName(DepthID), parameters.depth);  
  result &= writer.addValue(getName(WetID), parameters.wet);  
  result &= writer.addValue(getName(DryID), parameters.dry);    
  return result;
}

Result StereoPhaserEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(RateID), parameters.rate);  
  result &= reader.getValue(getName(DepthID), parameters.depth);  
  result &= reader.getValue(getName(WetID), parameters.wet);  
  result &= reader.getValue(getName(DryID), parameters.dry);  
  return result;
}

std::string StereoPhaserEffect::getID() const {
  return "ZA_StereoPhaserEffect";
}

int64_t StereoPhaserEffect::getVersion() const {
  return 1;
}

StereoPhaserEffect::Parameters StereoPhaserEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio