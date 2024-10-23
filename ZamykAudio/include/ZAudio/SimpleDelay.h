#pragma once

#include <ZAudio/CircularBuffer.h>
#include <ZAudio/CommonTypes.h>

namespace ZAudio::Tools {


class SimpleDelay {
public:
  SimpleDelay() = default;
  SimpleDelay(Frequency sampleRate_p, Time delayTime, Time reservedDelayTime);

  sample_t get() const;
  void push(sample_t in);
  void setDelayTime(Time delayTime);

  Time getDelayTime() const;
  double getDelayInSamples() const;

private:  
  Frequency sampleRate;  
  double delayInSamples = 0.;  
  CircularBuffer<sample_t> delay;  
};


} // namespace ZAudio::Tools