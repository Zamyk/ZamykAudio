#include <ZAudio/DynamicsProcessorEffect.h>

namespace ZAudio {

DynamicsProcessorEffect::Parameters::Parameters(Volume outputGain_p, Time attackTime_p, Time releaseTime_p, Volume threshold_p, double ratio_p, bool hardStop_p, Type type_p) :
    outputGain(outputGain_p), attackTime(attackTime_p), releaseTime(releaseTime_p), threshold(threshold_p), ratio(ratio_p), hardStop(hardStop_p), type(type_p) {}

DynamicsProcessorEffect::DynamicsProcessorEffect(Parameters parameters_p) : parameters(parameters_p) {}

void DynamicsProcessorEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  auto det = detector.process(in[0]);    
  if( (det > parameters.threshold && parameters.type == Type::Compressor) || (det < parameters.threshold && parameters.type == Type::Expander) ) {
    Volume gainChange = calculateGainChange(det);
    out[0] = in[0] * gainChange.linear() * parameters.outputGain.linear();            
  }
  else {
    out[0] = in[0];
  }
}  

void DynamicsProcessorEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case(OutputGainID):
      parameters.outputGain = value.getVolume();      
    break;

    case(AttackTimeID):
      parameters.attackTime = value.getTime();
      detector.setAttackTime(parameters.attackTime);
    break;

    case(ReleaseTimeID):
      parameters.releaseTime = value.getTime();
      detector.setReleaseTime(parameters.attackTime);
    break;

    case(ThresholdID):
      parameters.threshold = value.getVolume();      
    break;

    case(RatioID):
      parameters.ratio = value.getNonInteger();
    break;

    case(HardStopID):
      parameters.hardStop = value.getBoolean();
    break;

    case(TypeID):
      parameters.type = value.getEnum<Type>();
    break;

    default:
      assert(false);
  }
}

uint32_t DynamicsProcessorEffect::getTailTime() const {
  return 0;
}

void DynamicsProcessorEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  detector = Tools::AudioDetector(sampleRate, Tools::AudioDetector::DetectMode::RootMeanSquare, parameters.attackTime, parameters.releaseTime, false);
}

Volume DynamicsProcessorEffect::calculateGainChange(Volume inputVolume) {
  if(parameters.type == Type::Compressor) {
    if(parameters.hardStop) {
      Volume targetGain = parameters.threshold;
      Volume gainChange = Volume::dB(targetGain.dB() - inputVolume.dB());      
      return gainChange;
    }
    else {
      Volume targetGain = Volume::dB(parameters.threshold.dB() + ( inputVolume.dB() - parameters.threshold.dB() ) / parameters.ratio);
      Volume gainChange = Volume::dB(targetGain.dB() - inputVolume.dB());      
      return gainChange;
    }    
  }
  else if(parameters.type == Type::Expander) {
    if(parameters.hardStop) {
      Volume targetGain = Volume::linear(0);
      Volume gainChange = Volume::dB(targetGain.dB() - inputVolume.dB());      
      return gainChange;
    }
    else {
      Volume targetGain = Volume::dB(parameters.threshold.dB() + ( inputVolume.dB() - parameters.threshold.dB() ) * parameters.ratio);
      Volume gainChange = Volume::dB(targetGain.dB() - inputVolume.dB());      
      return gainChange;
    }  
  }
  assert(false);
  return Volume::dB(0);
}


std::unique_ptr<Effect> DynamicsProcessorEffect::clone() const {
  return std::make_unique<DynamicsProcessorEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, DynamicsProcessorEffect::NumOfParameters> names = {
    "OutputGain",
    "AttackTime",
    "ReleaseTime",
    "Threshold",
    "Ratio",
    "HardStop",
    "Type"
  };
  return names[ind];
}  

static const std::array<std::pair<DynamicsProcessorEffect::Type, std::string>, 2> enumNames = {
  std::make_pair(DynamicsProcessorEffect::Type::Compressor, "Compressor"),
  std::make_pair(DynamicsProcessorEffect::Type::Expander, "Expander")
};

Result DynamicsProcessorEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addValue(getName(OutputGainID), parameters.outputGain);
  result &= writer.addValue(getName(AttackTimeID), parameters.attackTime);
  result &= writer.addValue(getName(ReleaseTimeID), parameters.releaseTime);
  result &= writer.addValue(getName(ThresholdID), parameters.threshold);
  result &= writer.addValue(getName(RatioID), parameters.ratio);
  result &= writer.addValue(getName(HardStopID), parameters.hardStop);
  result &= writer.addEnumValue(getName(TypeID), parameters.type, enumNames);
  return result;
}

Result DynamicsProcessorEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();
  result &= reader.getValue(getName(OutputGainID), parameters.outputGain);
  result &= reader.getValue(getName(AttackTimeID), parameters.attackTime);
  result &= reader.getValue(getName(ReleaseTimeID), parameters.releaseTime);
  result &= reader.getValue(getName(ThresholdID), parameters.threshold);
  result &= reader.getValue(getName(RatioID), parameters.ratio);
  result &= reader.getValue(getName(HardStopID), parameters.hardStop);
  result &= reader.getEnumValue(getName(TypeID), parameters.type, enumNames);
  return result;
}

std::string DynamicsProcessorEffect::getID() const {
  return "ZA_DynamicsProcessorEffect";
}

int64_t DynamicsProcessorEffect::getVersion() const {
  return 1;
}

DynamicsProcessorEffect::Parameters DynamicsProcessorEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio