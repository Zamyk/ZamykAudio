#include <ZAudio/FilterEffect.h>

namespace ZAudio {


FilterEffect::Parameters::Parameters(Type type_p, Frequency frequency_p, double q_p, Volume gain_p) :
  type(type_p),
  frequency(frequency_p),
  q(q_p),
  gain(gain_p) {}

FilterEffect::FilterEffect(Parameters parameters_p) : parameters(parameters_p) {}

void FilterEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = filter.process(in[0]);
}

void FilterEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case TypeID:
      parameters.type = value.getEnum<Type>();
      break;

    case FrequencyID:
      parameters.frequency = value.getFrequency();
      break;

    case Q_ID:
      parameters.q = value.getNonInteger();
      break;
    
    case GainID:
      parameters.gain = value.getVolume();
      break;

    default:
      assert(false);
  }
  updateFilter();
}  

void FilterEffect::updateFilter() {    
  filter.reset(Tools::AnalogFilter::Parameters(parameters.type, sampleRate, parameters.frequency, parameters.q, parameters.gain));    
}

uint32_t FilterEffect::getTailTime() const {
  return 0;
}

void FilterEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  updateFilter();
} 

std::unique_ptr<Effect> FilterEffect::clone() const {
  return std::make_unique<FilterEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, FilterEffect::NumOfParameters> names = {
    "Type",
    "Frequency",
    "Q",
    "Gain"
  };
  return names[ind];
}  

Result FilterEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addEnumValue(getName(TypeID), parameters.type, Tools::AnalogFilter::typesToStrings);
  result &= writer.addValue(getName(FrequencyID), parameters.frequency);
  result &= writer.addValue(getName(Q_ID), parameters.q);
  result &= writer.addValue(getName(GainID), parameters.gain);  
  return result;
}

Result FilterEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();
  result &= reader.getEnumValue(getName(TypeID), parameters.type, Tools::AnalogFilter::typesToStrings);
  result &= reader.getValue(getName(FrequencyID), parameters.frequency);
  result &= reader.getValue(getName(Q_ID), parameters.q);
  result &= reader.getValue(getName(GainID), parameters.gain);
  return result;
}

std::string FilterEffect::getID() const {
  return "ZA_FilterEffect";
}

int64_t FilterEffect::getVersion() const {
  return 2;
}

FilterEffect::Parameters FilterEffect::getParameters() const {
  return parameters; 
}


} // namespace ZAudio