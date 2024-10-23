#include <ZAudio/WhisperEffect.h>

namespace ZAudio {

WhisperEffect::Parameters::Parameters(size_t frameSizeTwoPow_p, double hopRatio_p) : frameSizeTwoPow(frameSizeTwoPow_p), hopRatio(hopRatio_p) {}

WhisperEffect::WhisperEffect(Parameters parameters_p) : parameters(parameters_p), phaseVocoder(1 << parameters.frameSizeTwoPow, (1 << parameters.frameSizeTwoPow) * std::clamp(parameters_p.hopRatio, 0.1, 1.)) {}

void WhisperEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  phaseVocoder.push(in[0]);
  if(phaseVocoder.frameCompleted()) {
    auto& bins = phaseVocoder.getBins();
    for(auto& v : bins) {
      v = std::polar<double>(std::abs(v), dist(gen));
    }
  }
  out[0] = phaseVocoder.get();    
}

uint32_t WhisperEffect::getTailTime() const {
  return 1 << parameters.frameSizeTwoPow;
}

void WhisperEffect::setSampleRate(Frequency sampleRate_p) {
  *this = WhisperEffect(parameters);
}

void WhisperEffect::setParameter(size_t id, ParameterValue value) {
  switch (id) {    
    case HopRatioID:
      parameters.hopRatio = value.getNonInteger();
      phaseVocoder.setInputHopSize((1 << parameters.frameSizeTwoPow) * std::clamp(parameters.hopRatio, 0.1, 1.));
      phaseVocoder.setOutputHopSize((1 << parameters.frameSizeTwoPow) * std::clamp(parameters.hopRatio, 0.1, 1.));
      break;

    case FrameSizeTwoPowID:
      parameters.frameSizeTwoPow = value.getInteger();
      *this = WhisperEffect(Parameters(parameters));
      break;

    default:
      assert(false);
  }  
}

std::unique_ptr<Effect> WhisperEffect::clone() const {
  return std::make_unique<WhisperEffect>(parameters); // no copy constructor but should work still ok
}

Result WhisperEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue("FrameSizeTwoPow", parameters.frameSizeTwoPow);    
  result &= writer.addValue("HopRatio", parameters.hopRatio);      
  return result;
}

Result WhisperEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue("FrameSizeTwoPow", parameters.frameSizeTwoPow);
  result &= reader.getValue("HopRatio", parameters.hopRatio);      
  return result;
}

std::string WhisperEffect::getID() const {
  return "ZA_WhisperEffect";
}

int64_t WhisperEffect::getVersion() const {
  return 3;
}


} // namespace Zaudio
