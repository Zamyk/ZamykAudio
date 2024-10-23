#include <ZAudio/DelayEffect.h>

namespace ZAudio {


DelayEffect::Parameters::Parameters(Time delayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p) : delayTime(delayTime_p), dry(dry_p), wet(wet_p), feedback(feedback_p) {}

DelayEffect::DelayEffect(Parameters parameters_p) : parameters(parameters_p) {}

void DelayEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = delay.process(in[0]);
}

void DelayEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case DelayTimeID:
      parameters.delayTime = value.getTime();
      delay.setDelayTime(parameters.delayTime);
    break;

    case ReservedDelayTimeID:
      parameters.reservedDelayTime = value.getTime();
    break;

    case DryID:
      parameters.dry = value.getVolume();
      delay.setDry(parameters.dry);
    break;

    case WetID:
      parameters.wet = value.getVolume();
      delay.setWet(parameters.wet);
    break;

    case FeedbackID:
      parameters.feedback = value.getVolume();
      delay.setFeedback(parameters.feedback);
    break;

    default:
      assert(false);        
  }  
      
}

uint32_t DelayEffect::getTailTime() const {
  return std::ceil(delay.getRT60TimeInSamples());
}

void DelayEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  delay = Tools::AudioDelay(sampleRate, parameters.delayTime, parameters.reservedDelayTime, parameters.dry, parameters.wet, parameters.feedback);
}

std::unique_ptr<Effect> DelayEffect::clone() const {
  return std::make_unique<DelayEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, DelayEffect::NumOfParameters> names = {
    "DelayTime",
    "ReservedDelayTime",
    "Dry",
    "Wet",
    "Feedback"
  };
  return names[ind];
}  

Result DelayEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addValue(getName(DelayTimeID), parameters.delayTime);
  result &= writer.addValue(getName(ReservedDelayTimeID), parameters.reservedDelayTime);
  result &= writer.addValue(getName(DryID), parameters.dry);
  result &= writer.addValue(getName(WetID), parameters.wet);
  result &= writer.addValue(getName(FeedbackID), parameters.feedback);
  return result;
}

Result DelayEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();
  result &= reader.getValue(getName(DelayTimeID), parameters.delayTime);
  result &= reader.getValue(getName(ReservedDelayTimeID), parameters.reservedDelayTime);
  result &= reader.getValue(getName(DryID), parameters.dry);
  result &= reader.getValue(getName(WetID), parameters.wet);
  result &= reader.getValue(getName(FeedbackID), parameters.feedback);  
  return result;
}

std::string DelayEffect::getID() const {
  return "ZA_DelayEffect";
}

int64_t DelayEffect::getVersion() const {
  return 1;
}

DelayEffect::Parameters DelayEffect::getParameters() const {
  return parameters;
}


} // namespace ZAudio