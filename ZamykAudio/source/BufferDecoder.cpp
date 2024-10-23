#include <ZAudio/BufferDecoder.h>

namespace ZAudio {


BufferDecoder::BufferDecoder(SoundBuffer&& buffer_p) : buffer(std::make_shared<SoundBuffer>(std::move(buffer_p))) {}

BufferDecoder::BufferDecoder(std::shared_ptr<SoundBuffer> buffer_p) : buffer(buffer_p) {}

bool BufferDecoder::get(std::span<sample_t> out) {
  if(looped && position >= buffer->getLoopEnd()) {
    seek(buffer->getLoopStart());
  }
  else if(position >= buffer->getLength()) {
    return false;
  }  
  buffer->getFrame(position, out.begin());
  position++;
  return true;
}

void BufferDecoder::seek(uint64_t position_p) {
  position = position_p;
}

uint64_t BufferDecoder::getLength() {
  return buffer->getLength();
}

Frequency BufferDecoder::getSampleRate() {
  return buffer->getSampleRate();
}

uint64_t BufferDecoder::getPosition() {
  return position;
}

uint64_t BufferDecoder::getLoopStart() {
  return buffer->getLoopStart();
}

uint64_t BufferDecoder::getLoopEnd() {
  return buffer->getLoopEnd();
}

void BufferDecoder::setLooped(bool looped_p) {
  looped = looped_p;
}

bool BufferDecoder::errorOccured() const {
  return false;
}

FrameFormat BufferDecoder::getFormat() const {
  return buffer->getFrameFormat();
}


} // namespace ZAudio