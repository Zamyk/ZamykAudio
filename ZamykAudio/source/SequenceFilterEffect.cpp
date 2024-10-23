#include <ZAudio/SequenceFilterEffect.h>

namespace ZAudio {


SequenceFilterEffect::Parameters::Parameters(Frequency changeFrequency_p, Frequency minFrequency_p, Frequency maxFrequency_p, double filterQ_p) :
  changeFrequency(changeFrequency_p),
  minFrequency(minFrequency_p),
  maxFrequency(maxFrequency_p),
  filterQ(filterQ_p) {}

SequenceFilterEffect::SequenceFilterEffect(Parameters parameters_p) : parameters(parameters_p), dist(parameters.minFrequency.Hz(), parameters.maxFrequency.Hz()) {}

void SequenceFilterEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  counter++;
  if(counter >= periodInSamples) {      
    filter.reset(Tools::AnalogFilter::Parameters::createBandPassParameters(sampleRate, Frequency::Hz(dist(gen)), parameters.filterQ));
    counter = 0;            
  }
  out[0] = filter.process(in[0]);    
}

void SequenceFilterEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case ChangeFrequencyID:
      parameters.changeFrequency = value.getFrequency();
      periodInSamples = sampleRate.Hz() / parameters.changeFrequency.Hz();  
      break;
    case MinFrequencyID:
      parameters.minFrequency = value.getFrequency();
      break;
    case MaxFrequencyID:
      parameters.maxFrequency = value.getFrequency();
      break;
    case FilterQ_ID:
      parameters.filterQ = value.getNonInteger();
      updateFilter();
      break;
    default:
      assert(false);
  }
}

uint32_t SequenceFilterEffect::getTailTime() const {
  return 0;
}

void SequenceFilterEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;    
  updateFilter();
  periodInSamples = sampleRate.Hz() / parameters.changeFrequency.Hz();    
}

void SequenceFilterEffect::updateFilter() {
  filter = Tools::AnalogFilter(Tools::AnalogFilter::Parameters::createBandPassParameters(sampleRate, parameters.minFrequency, parameters.filterQ));
}

std::unique_ptr<Effect> SequenceFilterEffect::clone() const {
  return std::make_unique<SequenceFilterEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, SequenceFilterEffect::NumOfParameters> names = {
    "ChangeFrequency",
    "MinFrequency",
    "MaxFrequency",
    "FilterQ"
  };
  return names[ind];
}  

Result SequenceFilterEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();  
  result &= writer.addValue(getName(ChangeFrequencyID), parameters.changeFrequency);
  result &= writer.addValue(getName(MinFrequencyID), parameters.minFrequency);
  result &= writer.addValue(getName(MaxFrequencyID), parameters.maxFrequency);
  result &= writer.addValue(getName(FilterQ_ID), parameters.filterQ);
  return result;
}

Result SequenceFilterEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();    
  result &= reader.getValue(getName(ChangeFrequencyID), parameters.changeFrequency);
  result &= reader.getValue(getName(MinFrequencyID), parameters.minFrequency);
  result &= reader.getValue(getName(MaxFrequencyID), parameters.maxFrequency);
  result &= reader.getValue(getName(FilterQ_ID), parameters.filterQ);
  return result;
}

std::string SequenceFilterEffect::getID() const {
  return "ZA_SequenceFilterEffect";
}

int64_t SequenceFilterEffect::getVersion() const {
  return 1;
}

SequenceFilterEffect::Parameters SequenceFilterEffect::getParameters() const {
  return parameters;
}


} // namespace ZAudio