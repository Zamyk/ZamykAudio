#pragma once

#include <deque>
#include <string>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/AudioEncoder.h>
#include <ZAudio/SoundBuffer.h>

namespace ZAudio {


class BufferEncoder : public AudioEncoder {
public:    
  BufferEncoder(SoundBuffer& sound_p);
  FrameFormat getFormat() const override;
  Frequency getSampleRate() const override;
  void send(std::span<const sample_t> in) override;
  bool ended() const override;
  bool errorOccured() const override;
  
private:  
  SoundBuffer& sound;    
  size_t ind = 0;    
};


} // namespace ZAudio