#include <ZAudio/LooperEffect.h>

namespace ZAudio {


LooperEffect::Parameters::Parameters(Time maxDuration_p, Volume dry_p, Volume wet_p) :
  maxDuration(maxDuration_p),
  dry(dry_p),
  wet(wet_p) {}

LooperEffect::LooperEffect(Parameters parameters_p) : parameters(parameters_p) {}

void LooperEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = in[0] * parameters.dry.linear();
  if(mode == Mode::Recording && buffer.size()) {
    *curr = in[0];
    duration++;
    curr++;    
    if(duration == maxDuration) {
      mode = Mode::Playing;
    }
  }
  else if(mode == Mode::Overdubbing && buffer.size()) {
    out[0] += *curr * parameters.wet.linear();
    *curr += in[0];    
    curr++;
  }
  else if(mode == Mode::Playing && buffer.size()) {
    out[0] += *curr * parameters.wet.linear();
    curr++;
  }
}

void LooperEffect::
setParameter(size_t id, ParameterValue value) {
  switch (id) {  
    case MaxDurationID:
      parameters.maxDuration = value.getTime();
      if(maxDuration < duration) {
        buffer.resize(duration);
        curr = buffer.current();
      }
      break;

    case DryID:
      parameters.dry = value.getVolume();
      break;

    case WetID:
      parameters.wet = value.getVolume();
      break;

    case ModeID:
      if(value.getEnum<Mode>() == Mode::Recording) {
        clear();
      }
      if(value.getEnum<Mode>() == Mode::Playing && mode == Mode::Recording) {
        buffer.resize(duration);
        curr = buffer.current();
      }
      mode = value.getEnum<Mode>();
      break;

    case ClearID:
      clear();
      break;

    default:
      assert(false);
  }
}

void LooperEffect::clear() {  
  buffer.reset(maxDuration);
  curr = buffer.current();
  duration = 0;  
}

uint32_t LooperEffect::getTailTime() const {
  return 0;
}

void LooperEffect::setSampleRate(Frequency sampleRate) {
  maxDuration = sampleRate.Hz() * parameters.maxDuration.seconds();
  buffer.reset(maxDuration);
  curr = buffer.current();
}

std::unique_ptr<Effect> LooperEffect::clone() const {
  return std::make_unique<LooperEffect>(*this);
}

std::string LooperEffect::getID() const {
  return "ZA_LooperEffect";
}

int64_t LooperEffect::getVersion() const {
  return 1;
}


} // namespace ZAudio