#include <ZAudio/AutoWahEffect.h>


namespace ZAudio {

AutoWahEffect::Parameters::Parameters(Volume lowEnvelope_p, Volume highEnvelope_p, Time decay_p, Frequency minFrequency_p, Frequency maxFrequency_p, double q_p) :
  lowEnvelope(lowEnvelope_p),
  highEnvelope(highEnvelope_p),
  decay(decay_p),
  minFrequency(minFrequency_p),
  maxFrequency(maxFrequency_p),
  q(q_p) {}


void AutoWahEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {  
  Volume env = detector.process(in[0]);  
  env = std::clamp(env, parameters.lowEnvelope, parameters.highEnvelope);  
  currFrequency = Frequency::Hz(NormalizedValue(parameters.lowEnvelope.dB(), parameters.highEnvelope.dB(), env.dB()).bind(parameters.minFrequency.Hz(), parameters.maxFrequency.Hz()));  
  updateFilter();
  out[0] = bandPass.process(in[0]);
}

void AutoWahEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  detector = Tools::AudioDetector(sampleRate, Tools::AudioDetector::DetectMode::MeanSquare, parameters.decay, parameters.decay, true);      
  currFrequency = parameters.minFrequency;  
}

void AutoWahEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case LowEnvelopeID:
      parameters.lowEnvelope = value.getVolume();
      break;
    case HighEnvelopeID:
      parameters.highEnvelope = value.getVolume();
      break;
    case DecayID:
      parameters.decay = value.getTime();
      detector.setAttackTime(parameters.decay);
      detector.setReleaseTime(parameters.decay);
      break;
    case MinFrequencyID:
      parameters.minFrequency = value.getFrequency();
      break;
    case MaxFrequencyID:
      parameters.maxFrequency = value.getFrequency();
      break;
    case Q_ID:
      parameters.q = value.getNonInteger();      
      break;
    default:
      assert(false);
  }
}

void AutoWahEffect::updateFilter() {
  bandPass.reset(Tools::AnalogFilter::Parameters::createBandPassParameters(sampleRate, currFrequency, parameters.q));
}

uint32_t AutoWahEffect::getTailTime() const {
  return 0;
}

static std::string getName(uint32_t v) {
  const std::array<std::string, AutoWahEffect::NumOfParameters> names = {
    "LowEnvelope",
    "HighEnvelope",
    "Decay",
    "MinFrequency",
    "MaxFrequency",
    "FilterFactor_Q"
  };
  return names[v];
}

std::unique_ptr<Effect> AutoWahEffect::clone() const {
  return std::make_unique<AutoWahEffect>(*this);
}

Result AutoWahEffect::save(Tools::TreeDatabaseWriter writer) const {
  auto r = Result::success();
  r &= writer.addValue(getName(LowEnvelopeID), parameters.lowEnvelope);
  r &= writer.addValue(getName(HighEnvelopeID), parameters.highEnvelope);
  r &= writer.addValue(getName(DecayID), parameters.decay);
  r &= writer.addValue(getName(MinFrequencyID), parameters.minFrequency);
  r &= writer.addValue(getName(MaxFrequencyID), parameters.maxFrequency);
  r &= writer.addValue(getName(Q_ID), parameters.q);
  return r;
} 

Result AutoWahEffect::load(Tools::TreeDatabaseReader reader) {     
  auto r = Result::success();
  r &= reader.getValue(getName(LowEnvelopeID), parameters.lowEnvelope);
  r &= reader.getValue(getName(HighEnvelopeID), parameters.highEnvelope);
  r &= reader.getValue(getName(DecayID), parameters.decay);
  r &= reader.getValue(getName(MinFrequencyID), parameters.minFrequency);
  r &= reader.getValue(getName(MaxFrequencyID), parameters.maxFrequency);
  r &= reader.getValue(getName(Q_ID), parameters.q);    
  return r;
}

std::string AutoWahEffect::getID() const {
  return "ZA_AutoWahEffect";
}

int64_t AutoWahEffect::getVersion() const {
  return 1;
}

AutoWahEffect::Parameters AutoWahEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio