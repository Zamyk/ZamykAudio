#include <ZAudio/VolumeControlEffect.h>

namespace ZAudio {

VolumeControlEffect::VolumeControlEffect(Parameters parameters_p) : parameters(parameters_p), smoothedParameters(parameters_p) {}

void VolumeControlEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  smoothedParameters.volumeChange = volumeChangeSmoother.update();
  out[0] = in[0] * smoothedParameters.volumeChange.linear();
}

void VolumeControlEffect::setParameter(size_t id, ParameterValue value) {
  if(id == VolumeChangeID) {
    volumeChangeSmoother.setDestination(value.getVolume());
    parameters.volumeChange = value.getVolume();
  }
  if(id == SetVolumeChangeNoSmoothingID) {
    volumeChangeSmoother.setInstant(value.getVolume());
    parameters.volumeChange = value.getVolume();
  }
}

uint32_t VolumeControlEffect::getTailTime() const {
  return 0;
}

void VolumeControlEffect::setSampleRate(Frequency sampleRate_p) {
  volumeChangeSmoother = Tools::Smoother<Volume>(sampleRate_p, parameters.volumeChange, parameters.maxChangePerSecond);
  smoothedParameters = parameters;
}

std::unique_ptr<Effect> VolumeControlEffect::clone() const {
  return std::make_unique<VolumeControlEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, VolumeControlEffect::NumOfParameters> names = {
    "VolumeChange",
    "MaxChangePerSecond"
  };
  return names[ind];
}

Result VolumeControlEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addValue(getName(VolumeChangeID), parameters.volumeChange);
  result &= writer.addValue(getName(MaxChangePerSecondID), parameters.maxChangePerSecond);
  return result;
}

Result VolumeControlEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();
  result &= reader.getValue(getName(VolumeChangeID), parameters.volumeChange);
  result &= reader.getValue(getName(MaxChangePerSecondID), parameters.maxChangePerSecond);

  smoothedParameters = parameters;
  return result;
}

std::string VolumeControlEffect::getID() const {
  return "ZA_VolumeControlEffect";
}

int64_t VolumeControlEffect::getVersion() const {
  return 2;
}

VolumeControlEffect::Parameters VolumeControlEffect::getParameters() const {
  return parameters;
}


} // namespace ZAudio
