#include <ZAudio/TremoloEffect.h>

namespace ZAudio {

TremoloEffect::Parameters::Parameters(Frequency frequency_p, Volume minGain_p, Volume maxGain_p) :
  frequency(frequency_p),
  minGain(minGain_p),
  maxGain(maxGain_p) {}

TremoloEffect::TremoloEffect(Parameters parameters_p) : parameters(parameters_p) {}


void TremoloEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  auto lfoOut = lfo.get();
  out[0] = in[0] * Volume::dB(lfoOut.bind(parameters.minGain.dB(), parameters.maxGain.dB())).linear();    
}

void TremoloEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case FrequencyID:
      parameters.frequency = value.getFrequency();
      lfo.setFrequency(parameters.frequency);
      break;
    case MinGainID:
      parameters.minGain = value.getVolume();      
      break;
    case MaxGainID:
      parameters.maxGain = value.getVolume();
      break;
    default:
      assert(false);
  }
}  

uint32_t TremoloEffect::getTailTime() const {
  return 0;
}

void TremoloEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;    
  lfo = Tools::LowFrequencyOscillator(sampleRate, parameters.frequency, Tools::LowFrequencyOscillator::ShapeType::Sine);    
}  

std::unique_ptr<Effect> TremoloEffect::clone() const {
  return std::make_unique<TremoloEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, TremoloEffect::NumOfParameters> names = {
    "Frequency",
    "MinGain",
    "MaxGain"    
  };
  return names[ind];
}  

Result TremoloEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addValue(getName(FrequencyID), parameters.frequency);
  result &= writer.addValue(getName(MinGainID), parameters.minGain);
  result &= writer.addValue(getName(MaxGainID), parameters.maxGain);  
  return result;
}

Result TremoloEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(FrequencyID), parameters.frequency);
  result &= reader.getValue(getName(MinGainID), parameters.minGain);
  result &= reader.getValue(getName(MaxGainID), parameters.maxGain);
  return result;
}

std::string TremoloEffect::getID() const {
  return "ZA_TremoloEffect";
}

int64_t TremoloEffect::getVersion() const {
  return 1;
}

TremoloEffect::Parameters TremoloEffect::getParameters() const {
  return parameters;
}


} // namespace ZAudio