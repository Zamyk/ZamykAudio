#pragma once

#include <ZAudio/AudioDecoder.h>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/SoundBuffer.h>

namespace ZAudio {


class BufferDecoder : public AudioDecoder {
public:
  BufferDecoder(SoundBuffer&& buffer_p);
  BufferDecoder(std::shared_ptr<SoundBuffer> buffer_p);
  bool get(std::span<sample_t> out) override;
  void seek(uint64_t position) override;
  uint64_t getLength() override;
  Frequency getSampleRate() override;
  uint64_t getPosition() override;
  uint64_t getLoopStart() override;
  uint64_t getLoopEnd() override;
  void setLooped(bool looped_p) override;
  bool errorOccured() const override;
  FrameFormat getFormat() const override;  
private:
  bool looped = false;
  std::shared_ptr<SoundBuffer> buffer;  
  uint64_t position = 0;  
};

} // namespace ZAudio