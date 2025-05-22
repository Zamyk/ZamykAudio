#include <ZAudio/DuckDelayEffect.h>

namespace ZAudio::Tools {


DelayGainController::DelayGainController(Frequency sampleRate_p, Volume threshold_p, Time recoveryTime_p, Volume wetMin_p, Volume wetMax_p) :
  sampleRate(sampleRate_p),
  threshold(threshold_p),
  recoveryTime(recoveryTime_p),
  wetMin(wetMin_p),
  wetMax(wetMax_p),
  increaseDB(calculateIncrese()) {}


void DelayGainController::setThreshold(Volume threshold_p) {
  threshold = threshold_p;
}

void DelayGainController::setRecoveryTime(Time time_p) {
  recoveryTime = time_p;
  increaseDB = calculateIncrese();
}

void DelayGainController::setWetMin(Volume wetMin_p) {
  wetMin = wetMin_p;
  increaseDB = calculateIncrese();
}

void DelayGainController::setWetMax(Volume wetMax_p) {
  wetMax = wetMax_p;
  increaseDB = calculateIncrese();
}

Volume DelayGainController::process(Volume env) {  
  if(env > threshold) {
    current = wetMin;            
  }
  else {        
    double tmp = fmin(current.dB() + increaseDB, wetMax.dB());
    current = Volume::dB(tmp);          
  }  
  return current;
}

double DelayGainController::calculateIncrese() const {
  return ( wetMax.dB() - wetMin.dB() ) / ( sampleRate.Hz() * recoveryTime.seconds() );
}


} // namespace ZAudio::Tools


namespace ZAudio {


DuckDelayEffect::Parameters::Parameters(Time delayTime_p) : 
  delayTime(delayTime_p) {}

DuckDelayEffect::Parameters::Parameters(Time delayTime_p, Volume dry_p, Volume feedback_p) :
  delayTime(delayTime_p), 
  dry(dry_p),
  feedback(feedback_p) {}

DuckDelayEffect::Parameters::Parameters(Time delayTime_p, Volume dry_p, Volume feedback_p, Volume wetMin_p, Volume wetMax_p, Volume threshold_p, Time recoveryTime_p) :
  delayTime(delayTime_p),
  dry(dry_p),
  feedback(feedback_p),
  wetMin(wetMin_p),
  wetMax(wetMax_p),
  threshold(threshold_p),
  recoveryTime(recoveryTime_p) {}


void DuckDelayEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  auto env = detector.process(in[0]);
  delay.setWet(gainControl.process(env));
  out[0] = delay.process(in[0]);
}

uint32_t DuckDelayEffect::getTailTime() const {
  return std::ceil(delay.getRT60TimeInSamples());
}

void DuckDelayEffect::setParameter(size_t id, ParameterValue value) {
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

    case FeedbackID:
      parameters.feedback = value.getVolume();
      delay.setFeedback(parameters.feedback);
    break;

    case WetMinID:
      parameters.wetMin = value.getVolume();      
      gainControl.setWetMin(parameters.wetMin);
    break;

    case WetMaxID:
      parameters.wetMax = value.getVolume();
      gainControl.setWetMax(parameters.wetMax);
    break;

    case ThresholdID:
      parameters.threshold = value.getVolume();
      gainControl.setThreshold(parameters.threshold);
    break;

    case RecoveryTimeID:
      parameters.recoveryTime = value.getTime();
      gainControl.setRecoveryTime(parameters.recoveryTime);
    break;

    case AttackTimeID:
      parameters.attackTime = value.getTime();      
      detector.setAttackTime(parameters.attackTime);
    break;    

    case ReleaseTimeID:
      parameters.releaseTime = value.getTime();
      detector.setReleaseTime(parameters.releaseTime);      
    break;    
    
    default:
      assert(false);
  }
}

void DuckDelayEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  delay = Tools::AudioDelay(sampleRate, parameters.delayTime, parameters.reservedDelayTime, parameters.dry, parameters.wetMax, parameters.feedback);
  detector = Tools::AudioDetector(sampleRate, Tools::AudioDetector::DetectMode::MeanSquare, parameters.attackTime, parameters.releaseTime, true);
  gainControl = Tools::DelayGainController(sampleRate, parameters.threshold, parameters.recoveryTime, parameters.wetMin, parameters.wetMax);
}

std::unique_ptr<Effect> DuckDelayEffect::clone() const {
  return std::make_unique<DuckDelayEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, DuckDelayEffect::NumOfParameters> names = {
    "DelayTime",
    "ReservedDelayTime",
    "Dry",
    "Feedback",
    "WetMin",
    "WetMax",
    "Threshold",
    "RecoveryTime",
    "AttackTime",
    "ReleaseTime"
  };
  return names[ind];
}  

Result DuckDelayEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addValue(getName(DelayTimeID), parameters.delayTime);  
  result &= writer.addValue(getName(ReservedDelayTimeID), parameters.reservedDelayTime);  
  result &= writer.addValue(getName(DryID), parameters.dry);  
  result &= writer.addValue(getName(FeedbackID), parameters.feedback);  
  result &= writer.addValue(getName(WetMinID), parameters.wetMin);  
  result &= writer.addValue(getName(WetMaxID), parameters.wetMax);  
  result &= writer.addValue(getName(ThresholdID), parameters.threshold);  
  result &= writer.addValue(getName(RecoveryTimeID), parameters.recoveryTime);  
  result &= writer.addValue(getName(AttackTimeID), parameters.attackTime);  
  result &= writer.addValue(getName(ReleaseTimeID), parameters.releaseTime);  
  return result;
}

Result DuckDelayEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();
  result &= reader.getValue(getName(DelayTimeID), parameters.delayTime);  
  result &= reader.getValue(getName(ReservedDelayTimeID), parameters.reservedDelayTime);  
  result &= reader.getValue(getName(DryID), parameters.dry);  
  result &= reader.getValue(getName(FeedbackID), parameters.feedback);  
  result &= reader.getValue(getName(WetMinID), parameters.wetMin);  
  result &= reader.getValue(getName(WetMaxID), parameters.wetMax);  
  result &= reader.getValue(getName(ThresholdID), parameters.threshold);  
  result &= reader.getValue(getName(RecoveryTimeID), parameters.recoveryTime);  
  result &= reader.getValue(getName(AttackTimeID), parameters.attackTime);  
  result &= reader.getValue(getName(ReleaseTimeID), parameters.releaseTime);  
  return result;
}

std::string DuckDelayEffect::getID() const {
  return "ZA_DuckDelayEffect";
}

int64_t DuckDelayEffect::getVersion() const {
  return 1;
}

DuckDelayEffect::Parameters DuckDelayEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio