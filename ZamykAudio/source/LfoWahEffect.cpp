#include <ZAudio/LfoWahEffect.h>

namespace ZAudio {


LfoWahEffect::Parameters::Parameters(Frequency rate_p, Frequency minFrequency_p, Frequency maxFrequency_p) :
  rate(rate_p),
  minFrequency(minFrequency_p),
  maxFrequency(maxFrequency_p) {}

LfoWahEffect::LfoWahEffect(Parameters parameters_p) : parameters(parameters_p) {}

void LfoWahEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  currFrequency = Frequency::Hz(lfo.get().bind(parameters.minFrequency.Hz(), parameters.maxFrequency.Hz()));
  updateFilter();
  out[0] = bandPass.process(in[0]);
}

void LfoWahEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case RateID:
      parameters.rate = value.getFrequency();
      lfo.setFrequency(parameters.rate);
      break;
    case MinFrequencyID:
      parameters.minFrequency = value.getFrequency();      
      break;
    case MaxFrequencyID:
      parameters.maxFrequency = value.getFrequency();
      break;
    default:
      assert(false);
  }
}

uint32_t LfoWahEffect::getTailTime() const {
  return 0;
}

void LfoWahEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;  
  lfo = Tools::LowFrequencyOscillator(sampleRate, parameters.rate, Tools::LowFrequencyOscillator::ShapeType::Sine);
  currFrequency = parameters.minFrequency;  
}

void LfoWahEffect::updateFilter() {
  bandPass.reset(ZAudio::Tools::AnalogFilter::Parameters::createBandPassParameters(sampleRate, currFrequency, 2));
}

std::unique_ptr<Effect> LfoWahEffect::clone() const {
  return std::make_unique<LfoWahEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, LfoWahEffect::NumOfParameters> names = {
    "Rate",
    "MinFrequency",
    "MaxFrequency"  
  };
  return names[ind];
}  

Result LfoWahEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue(getName(RateID), parameters.rate);
  result &= writer.addValue(getName(MinFrequencyID), parameters.minFrequency);
  result &= writer.addValue(getName(MaxFrequencyID), parameters.maxFrequency);
  return result;
}

Result LfoWahEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(RateID), parameters.rate);
  result &= reader.getValue(getName(MinFrequencyID), parameters.minFrequency);
  result &= reader.getValue(getName(MaxFrequencyID), parameters.maxFrequency);  
  setSampleRate(sampleRate);  
  return result;
}

std::string LfoWahEffect::getID() const {
  return "ZA_LfoWahEffect";
}

int64_t LfoWahEffect::getVersion() const {
  return 1;
}

LfoWahEffect::Parameters LfoWahEffect::getParameters() const {
  return parameters;
}


} // namespac ZAudio