#pragma once

#include <string>
#include <span>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/FrameFormat.h>

namespace ZAudio {


class AudioInput {
public:
  virtual ~AudioInput() = default;
  virtual void get(std::span<sample_t> out) = 0;
  virtual void setSampleRate(Frequency sampleRate) = 0;
  virtual void setParameter(size_t id, ParameterValue value) {}
  virtual ParameterValue getOutputValue(size_t id) const { return ParameterValue(); }  
  virtual bool errorOccured() const = 0;
  virtual bool isPlaying() const = 0;  
  virtual FrameFormat getFormat() const = 0;
};


} // namespace ZAudio