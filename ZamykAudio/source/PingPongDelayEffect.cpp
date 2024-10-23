#include <ZAudio/PingPongDelayEffect.h>

namespace ZAudio {


PingPongDelayEffect::Parameters::Parameters(Time leftDelayTime_p, Time rightDelayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p) : 
  leftDelayTime(leftDelayTime_p),
  rightDelayTime(rightDelayTime_p),
  dry(dry_p),
  wet(wet_p),
  feedback(feedback_p) {}

PingPongDelayEffect::Parameters::Parameters(Time leftDelayTime_p, Time rightDelayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p, Time reservedDelayTime_p) : 
  leftDelayTime(leftDelayTime_p),
  rightDelayTime(rightDelayTime_p),
  dry(dry_p),
  wet(wet_p),
  feedback(feedback_p) {}    

PingPongDelayEffect::PingPongDelayEffect(Parameters parameters_p) :
  parameters(parameters_p) {}  

void PingPongDelayEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = leftDelay.process(in[0]);
  out[1] = rightDelay.process(in[0]);        
}

void PingPongDelayEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case LeftDelayTimeID:
      parameters.leftDelayTime = value.getTime();
      leftDelay.setDelayTime(parameters.leftDelayTime);
      break;
    case RightDelayTimeID:
      parameters.rightDelayTime = value.getTime();
      rightDelay.setDelayTime(parameters.leftDelayTime);
      break;
    case ReservedDelayTimeID:
      parameters.reservedDelayTime = value.getTime();
      break;
    case DryID:
      parameters.dry = value.getVolume();
      leftDelay.setDry(parameters.dry);
      rightDelay.setDry(parameters.dry);
      break;
    case WetID:
      parameters.wet = value.getVolume();
      leftDelay.setWet(parameters.wet);
      rightDelay.setWet(parameters.wet);
      break;
    case FeedbackID:
      parameters.feedback = value.getVolume();
      leftDelay.setFeedback(parameters.feedback);
      rightDelay.setFeedback(parameters.feedback);
      break;
    default:
      assert(false);
  }
}

uint32_t PingPongDelayEffect::getTailTime() const {
  return std::ceil(std::max(leftDelay.getRT60TimeInSamples(), rightDelay.getRT60TimeInSamples()));
}

void PingPongDelayEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  leftDelay = Tools::AudioDelay(sampleRate, parameters.leftDelayTime, parameters.reservedDelayTime, parameters.dry, parameters.wet, parameters.feedback);
  rightDelay = Tools::AudioDelay(sampleRate, parameters.rightDelayTime, parameters.reservedDelayTime, parameters.dry, parameters.wet, parameters.feedback);
}

std::unique_ptr<Effect> PingPongDelayEffect::clone() const {
  return std::make_unique<PingPongDelayEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, PingPongDelayEffect::NumOfParameters> names = {
    "LeftDelayTime",
    "RightDelayTime",
    "ReservedDealyTime",
    "Dry",
    "Wet",
    "Feedback"
  };
  return names[ind];
}  

Result PingPongDelayEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue(getName(LeftDelayTimeID), parameters.leftDelayTime);
  result &= writer.addValue(getName(RightDelayTimeID), parameters.rightDelayTime);
  result &= writer.addValue(getName(ReservedDelayTimeID), parameters.reservedDelayTime);
  result &= writer.addValue(getName(DryID), parameters.dry);
  result &= writer.addValue(getName(WetID), parameters.wet);
  result &= writer.addValue(getName(FeedbackID), parameters.feedback);
  return result;
}

Result PingPongDelayEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(LeftDelayTimeID), parameters.leftDelayTime);
  result &= reader.getValue(getName(RightDelayTimeID), parameters.rightDelayTime);
  result &= reader.getValue(getName(ReservedDelayTimeID), parameters.reservedDelayTime);
  result &= reader.getValue(getName(DryID), parameters.dry);
  result &= reader.getValue(getName(WetID), parameters.wet);
  result &= reader.getValue(getName(FeedbackID), parameters.feedback);
  return result;
}

std::string PingPongDelayEffect::getID() const {
  return "ZA_PingPongDelayEffect";
}

int64_t PingPongDelayEffect::getVersion() const {
  return 1;
}

PingPongDelayEffect::Parameters PingPongDelayEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio