#include <ZAudio/ReverseDelayEffect.h>

namespace ZAudio {


ReverseDelayEffect::Parameters::Parameters(Time delayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p) :
 delayTime(delayTime_p),
 dry(dry_p),
 wet(wet_p),
 feedback(feedback_p) {}


ReverseDelayEffect::ReverseDelayEffect(Parameters parameters_p) : parameters(parameters_p) {}

void ReverseDelayEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  sample_t y = *curr;
  curr++;
  buffer.push(parameters.feedback.linear() * y + in[0]);
  out[0] = parameters.wet.linear() * y + parameters.dry.linear() * in[0];
}

void ReverseDelayEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case DelayTimeID:
      parameters.delayTime = value.getTime();
      buffer.resize(parameters.delayTime.seconds() * sampleRate.Hz() + 1);
      if(!curr.valid()) {
        curr = buffer.current();
      }
      break;
    case DryID:
      parameters.dry = value.getVolume();
      break;
    case WetID:
      parameters.wet = value.getVolume();
      break;
    case FeedbackID:
      parameters.feedback = value.getVolume();
      break;
    default:
      assert(false);
  }
}

uint32_t ReverseDelayEffect::getTailTime() const {
  return parameters.delayTime.seconds() * sampleRate.Hz();
}

void ReverseDelayEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  buffer.reset(parameters.delayTime.seconds() * sampleRate.Hz() + 1);
  curr = buffer.current();  
}

std::unique_ptr<Effect> ReverseDelayEffect::clone() const {
  return std::make_unique<ReverseDelayEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, ReverseDelayEffect::NumOfParameters> names = {
    "DelayTime",
    "Dry",
    "Wet",
    "Feedback"
  };
  return names[ind];
}  

Result ReverseDelayEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue(getName(DelayTimeID), parameters.delayTime);
  result &= writer.addValue(getName(DryID), parameters.dry);
  result &= writer.addValue(getName(WetID), parameters.wet);
  result &= writer.addValue(getName(FeedbackID), parameters.feedback);
  return result;
}

Result ReverseDelayEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(DelayTimeID), parameters.delayTime);
  result &= reader.getValue(getName(DryID), parameters.dry);
  result &= reader.getValue(getName(WetID), parameters.wet);
  result &= reader.getValue(getName(FeedbackID), parameters.feedback);
  return result;
}

std::string ReverseDelayEffect::getID() const {
  return "ZA_ReverseDelayEffect";
}

int64_t ReverseDelayEffect::getVersion() const {
  return 1;
}

ReverseDelayEffect::Parameters ReverseDelayEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio