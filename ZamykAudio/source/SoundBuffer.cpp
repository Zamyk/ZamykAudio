#include <ZAudio/SoundBuffer.h>

namespace ZAudio {


SoundBuffer::SoundBuffer(Frequency sampleRate_p, FrameFormat frameFormat_p, size_t length_p) : SoundBuffer(sampleRate_p, frameFormat_p, length_p, 0, length_p) {}

SoundBuffer::SoundBuffer(Frequency sampleRate_p, FrameFormat frameFormat_p, size_t length_p, size_t loopStart_p, size_t loopEnd_p) :
  sampleRate(sampleRate_p),
  frameFormat(frameFormat_p),
  samples(Tools::numberOfChannels(frameFormat_p), length_p),
  loopStart(loopStart_p),
  loopEnd(loopEnd_p) {}

void SoundBuffer::setSample(size_t x, size_t channel, sample_t sample) {
  samples.get(channel, x) = sample;
}

sample_t SoundBuffer::getSample(size_t x, size_t channel, sample_t sample) const {
  return samples.get(channel, x);
}

FrameFormat SoundBuffer::getFrameFormat() const {
  return frameFormat;
}

Frequency SoundBuffer::getSampleRate() const {
  return sampleRate;
}

size_t SoundBuffer::getLength() const {
  return samples.getNumOfCollumns();
}

size_t SoundBuffer::getNumberOfChannels() const {
  return samples.getNumOfRows();
}

size_t SoundBuffer::getLoopStart() const {
  return loopStart;
}

size_t SoundBuffer::getLoopEnd() const {
  return loopEnd; 
}


} // namespace ZAudio
