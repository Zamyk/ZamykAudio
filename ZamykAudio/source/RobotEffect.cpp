#include <ZAudio/RobotEffect.h>


namespace ZAudio {


RobotEffect::Parameters::Parameters(size_t frameSizeTwoPow_p, double hopRatio_p) : frameSizeTwoPow(frameSizeTwoPow_p), hopRatio(hopRatio_p) {}

RobotEffect::RobotEffect(Parameters parameters_p) : parameters(parameters_p), phaseVocoder(1 << parameters.frameSizeTwoPow, (1 << parameters.frameSizeTwoPow) * std::clamp(parameters_p.hopRatio, 0.1, 1.)) {}
  
void RobotEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {  
  phaseVocoder.push(in[0]);
  if(phaseVocoder.frameCompleted()) {
    auto& bins = phaseVocoder.getBins();
    for(auto& v : bins) {
      v = std::polar<double>(std::abs(v), 0);
    }
  }
  out[0] = phaseVocoder.get();    
}

void RobotEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case HopRatioID:
      parameters.hopRatio = value.getNonInteger();
      phaseVocoder.setInputHopSize((1 << parameters.frameSizeTwoPow) * std::clamp(parameters.hopRatio, 0.1, 1.));
      phaseVocoder.setOutputHopSize((1 << parameters.frameSizeTwoPow) * std::clamp(parameters.hopRatio, 0.1, 1.));
      break;

    case FrameSizeTwoPowID:
      parameters.frameSizeTwoPow = value.getInteger();
      *this = RobotEffect(Parameters(parameters));
      break;

    default:
      assert(false);
  }
}

uint32_t RobotEffect::getTailTime() const {
  return 1 << parameters.frameSizeTwoPow;
}

void RobotEffect::setSampleRate(Frequency sampleRate_p) {    
  *this = RobotEffect(parameters);
}

std::unique_ptr<Effect> RobotEffect::clone() const {
  return std::make_unique<RobotEffect>(parameters); // no copy constructor but should work still ok
}

Result RobotEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue("FrameSizeTwoPow", parameters.frameSizeTwoPow);    
  result &= writer.addValue("HopRatio", parameters.hopRatio);      
  return result;
}

Result RobotEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue("FrameSizeTwoPow", parameters.frameSizeTwoPow);
  result &= reader.getValue("HopRatio", parameters.hopRatio);      
  return result;
}

std::string RobotEffect::getID() const {
  return "ZA_RobotEffect";
}

int64_t RobotEffect::getVersion() const {
  return 3;
}

RobotEffect::Parameters RobotEffect::getParameters() const {
  return parameters;
}


} // namespace ZAudio