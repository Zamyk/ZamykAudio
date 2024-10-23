#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/LowFrequencyOscillator.h>
#include <ZAudio/AudioDelay.h>

namespace ZAudio::Tools {


class ModulatedDelay {
public:
struct Parameters {
  Frequency rate;
  Time minDelay;
  Time maxDelay;
  Volume wet;
  Volume dry;
  Volume feedback;  
  Time reservedDelay;
  LowFrequencyOscillator::ShapeType shape = LowFrequencyOscillator::ShapeType::Sine;
  double depth = 1.;
  double phaseOffset = 0.;    
};

  ModulatedDelay() = default;
  ModulatedDelay(Frequency sampleRate_p, Parameters params);

  void setRate(Frequency rate);
  void setMinDelay(Time time);
  void setMaxDelay(Time time);
  void setWet(Volume wet);
  void setDry(Volume dry);
  void setFeedback(Volume feedback);
  void setShape(LowFrequencyOscillator::ShapeType shape);
  void setDepth(double depth_p);
  sample_t process(sample_t in);
  uint32_t getTailTime() const;

private:
  Frequency sampleRate;    
  Parameters parameters;
  LowFrequencyOscillator lfo;
  AudioDelay delay;  
};


} // namespace