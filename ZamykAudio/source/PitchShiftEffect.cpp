#include <ZAudio/PitchShiftEffect.h>

namespace ZAudio {

PitchShiftEffect::Parameters::Parameters(double pitchShiftRatio_p, Type algorithm_p) : 
  pitchShiftRatio(pitchShiftRatio_p),
  algorithm(algorithm_p) {}

PitchShiftEffect::PitchShiftEffect(Parameters parameters_p) : 
  parameters(parameters_p),
  outputHopSize(inputHopSize * parameters_p.pitchShiftRatio),
  phaseVocoder(frameSize, inputHopSize, outputHopSize, parameters_p.algorithm),
  outputBuffer(inputHopSize) 
{        
  helper.reserve(frameSize);    
}  

void PitchShiftEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {    
  out[0] = outputBuffer[outInd++];
  if(outInd == outputBuffer.size()) {
    outInd = 0;
  }    
  inputBuffer.push_back(in[0]);
  if(inputBuffer.size() == inputHopSize) {      
    for(sample_t sample : inputBuffer) {
      phaseVocoder.push(sample);
    }
    if(phaseVocoder.frameCompleted()) {        
      outInd = 0;        
      for(size_t i = 0; i < outputHopSize; i++) {
        helper.push_back(phaseVocoder.get()) ;          
      }              
      Tools::convertSampleRateLinearInterpolation(helper, outputBuffer, helper.size(), outputBuffer.size());
      helper.clear();
    }
    inputBuffer.clear();
  }    
}

void PitchShiftEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case PitchShiftRatioID:
      parameters.pitchShiftRatio = value.getNonInteger();
      outputHopSize = inputHopSize * parameters.pitchShiftRatio;
      phaseVocoder.setOutputHopSize(outputHopSize);
      break;
    case AlgorithmID:
      parameters.algorithm = value.getEnum<Type>();      
      phaseVocoder.setPhaseCorrectionAlgorithm(parameters.algorithm);
      break;
    default:
      assert(false);
  }
}

uint32_t PitchShiftEffect::getTailTime() const {
  return frameSize;
}

void PitchShiftEffect::setSampleRate(Frequency sampleRate_p) {
  *this = PitchShiftEffect(parameters);
}

std::unique_ptr<Effect> PitchShiftEffect::clone() const {
  return std::make_unique<PitchShiftEffect>(parameters); // no copy constructor, but should work still ok
}

static std::string getName(uint32_t ind) {
  std::array<std::string, PitchShiftEffect::NumOfParameters> names = {
    "PitchShiftRatio",
    "Algorithm"
  };
  return names[ind];
}  


static const std::array<std::pair<PitchShiftEffect::Type, std::string>, 3> enumNames = {
  std::make_pair(PitchShiftEffect::Type::Normal, "Normal"),  
  std::make_pair(PitchShiftEffect::Type::PhaseLock, "PhaseLock"),
  std::make_pair(PitchShiftEffect::Type::PhaseTrack, "PhaseTrack")
};

Result PitchShiftEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue(getName(PitchShiftRatioID), parameters.pitchShiftRatio);  
  result &= writer.addEnumValue(getName(AlgorithmID), parameters.algorithm, enumNames);    
  return result;
}

Result PitchShiftEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(PitchShiftRatioID), parameters.pitchShiftRatio);  
  result &= reader.getEnumValue(getName(AlgorithmID), parameters.algorithm, enumNames);    
  return result;
}

std::string PitchShiftEffect::getID() const {
  return "ZA_PitchShiftEffect";
}

int64_t PitchShiftEffect::getVersion() const {
  return 1;
}

PitchShiftEffect::Parameters PitchShiftEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio