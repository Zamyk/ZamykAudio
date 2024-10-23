#include <ZAudio/LowFrequencyOscillator.h>
#include <numbers>

namespace ZAudio::Tools {


LowFrequencyOscillator::LowFrequencyOscillator(Frequency sampleRate_p, Frequency frequency_p, ShapeType shapeType_p, double phaseOffset) : 
  sampleRate(sampleRate_p),
  frequency(frequency_p),
  shapeType(shapeType_p),
  phase(phaseOffset),
  step(calculateStep()) {}

void LowFrequencyOscillator::setFrequency(Frequency frequency_p) {
  frequency = frequency_p;
  step = calculateStep();
}

void LowFrequencyOscillator::setShape(ShapeType shapeType_p) {
  shapeType = shapeType_p;
}

NormalizedValue LowFrequencyOscillator::get() {
  double tmp = phase;

  phase += step;
  if(phase > 1.) {
    phase -= 1.;
  }
  double v;
  if(shapeType == ShapeType::Saw) {
    v = tmp;
  }
  else if(shapeType == ShapeType::Sine) {
    v = (cos(2 * std::numbers::pi * tmp) / 2. + 0.5);
  }
  else if(shapeType == ShapeType::Triangle) {
    v = (2 * std::abs(tmp - 0.5));
  }
  else {
    assert(false);
  }  
  return NormalizedValue(0., 1., v);
}

double LowFrequencyOscillator::calculateStep() {
  return frequency.Hz() / sampleRate.Hz();
}


} // namespace ZAudio::Tools