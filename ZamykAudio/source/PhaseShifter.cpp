#include <ZAudio/PhaseShifter.h>

namespace ZAudio::Tools {


PhaseShifter::PhaseShifter(Frequency sampleRate_p, double depth_p, Frequency rate_p, double phaseOffset_p) : 
  sampleRate(sampleRate_p),
  depth(depth_p),
  lfo(sampleRate, rate_p, ZAudio::Tools::LowFrequencyOscillator::ShapeType::Sine, phaseOffset_p) {}

void PhaseShifter::setDepth(double depth_p) {
  depth = depth_p;  
}

void PhaseShifter::setRate(Frequency rate_p) {
  lfo.setFrequency(rate_p);
}

sample_t PhaseShifter::process(sample_t in) {
  auto lfoV = lfo.get();
  AnalogFilter::Parameters parameters;
  parameters.sampleRate = sampleRate;
  parameters.type = AnalogFilter::Type::AllPass;
  for(size_t i = 0; i < NumOfFilters; i++) {    
    parameters.frequency = Frequency::Hz(lfoV.bindFromLeft(minFrequencies[i].Hz(), maxFrequencies[i].Hz(), depth));
    filters[i].reset(parameters);
  } 

  sample_t out = in;
  for(size_t i = 0; i < NumOfFilters; i++) {
    out = filters[i].process(out);
  }

  return out;
}


} // namespace ZAudio::Tools