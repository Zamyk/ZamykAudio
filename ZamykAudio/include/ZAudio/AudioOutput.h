#pragma once

#include <string>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/FrameFormat.h>

namespace ZAudio {


class AudioOutput {
public:
  virtual ~AudioOutput() {}  
  virtual void send(std::span<const sample_t> in) = 0;
  virtual void setSampleRate(Frequency sampleRate) = 0;
  virtual void setParameter(size_t id, ParameterValue value) {}
  virtual ParameterValue getOutputValue(size_t id) { return ParameterValue(); }
  virtual bool errorOccured() const = 0;
  virtual bool ended() const = 0;  
  virtual FrameFormat getFormat() const = 0;
};


} // namespace ZAudio