#pragma once

#include <ZAudio/CommonTypes.h>

namespace ZAudio::Tools {


class LowFrequencyOscillator {
public:  
enum struct ShapeType {
  Saw, Sine, Triangle
};

  LowFrequencyOscillator() = default;
  LowFrequencyOscillator(Frequency sampleRate_p, Frequency frequency_p, ShapeType shapeType_p, double phaseOffset = 0.);

  void setFrequency(Frequency frequency_p);
  void setShape(ShapeType shapeType_p);

  NormalizedValue get();

private:
  Frequency sampleRate;
  Frequency frequency;  
  ShapeType shapeType = ShapeType::Sine;

  double phase = 0.;
  double step = 0.;

  double calculateStep();
};



} // namespace ZAudio::Tools