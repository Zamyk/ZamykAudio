#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/FrameFormat.h>
#include <ZAudio/TwoDimVector.h>

namespace ZAudio {


class SoundBuffer {
public:
  SoundBuffer() = default;
  SoundBuffer(Frequency sampleRate_p, FrameFormat frameFormat_p, size_t length_p);
  SoundBuffer(Frequency sampleRate_p, FrameFormat frameFormat_p, size_t length_p, size_t loopStart_p, size_t loopEnd_p);

  template <typename It>
  void setFrame(size_t x, It begin) {
    std::copy(begin, begin + samples.getNumOfRows(), samples.getCollumnBegin(x));
  }

  template<typename It>
  void getFrame(size_t x, It it) const {
    std::copy(samples.getCollumnCbegin(x), samples.getCollumnCend(x), it);
  }

  void setSample(size_t x, size_t channel, sample_t sample);
  sample_t getSample(size_t x, size_t channel, sample_t sample) const;

  FrameFormat getFrameFormat() const;
  Frequency getSampleRate() const;
  size_t getLength() const;
  size_t getNumberOfChannels() const;
  size_t getLoopStart() const;
  size_t getLoopEnd() const;

private:
  FrameFormat frameFormat = FrameFormat::None;
  Frequency sampleRate;
  Tools::TwoDimVector<sample_t> samples;
  size_t loopStart = 0;
  size_t loopEnd = 0;
};


} // namespace ZAudio