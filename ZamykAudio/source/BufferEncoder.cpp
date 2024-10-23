#include <ZAudio/BufferEncoder.h>

namespace ZAudio {


BufferEncoder::BufferEncoder(SoundBuffer& sound_p) : sound(sound_p) {}  

FrameFormat BufferEncoder::getFormat() const {
  return sound.getFrameFormat();
}

Frequency BufferEncoder::getSampleRate() const {
  return sound.getSampleRate();
}

void BufferEncoder::send(std::span<const sample_t> in) {
  if(ind < sound.getLength()) {
    for(size_t channel = 0; channel < sound.getNumberOfChannels(); channel++) {        
      sound.setSample(ind, channel, in[channel]);        
    }
    ind++;
  }
}  

bool BufferEncoder::ended() const {
  return ind == sound.getLength();
}
  
bool BufferEncoder::errorOccured() const {
  return false;
}


} // namespace ZAudio