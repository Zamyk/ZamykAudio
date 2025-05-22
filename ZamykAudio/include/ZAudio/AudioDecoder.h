#pragma once

#include <ZAudio/AudioInput.h>
#include <ZAudio/SoundBuffer.h>
#include <ZAudio/SampleRateConversion.h>
#include <ZAudio/ReaderWriterQueue.h>
#include <ZAudio/Smoother.h>

namespace ZAudio {

class AudioDecoder {
public:
  virtual ~AudioDecoder() = default;
  virtual bool get(std::span<sample_t> out) = 0;
  virtual void seek(uint64_t position) = 0;
  virtual uint64_t getLength() = 0;
  virtual Frequency getSampleRate() = 0;
  virtual uint64_t getPosition() = 0;

  virtual uint64_t getLoopStart() {
    return 0;
  }

  virtual uint64_t getLoopEnd() {
    return getLength();
  }

  virtual void setLooped(bool looped_p) = 0;

  virtual bool errorOccured() const = 0;
  virtual FrameFormat getFormat() const = 0;
};

class AsyncDecoder : public AudioDecoder {
public:
  AsyncDecoder(std::unique_ptr<AudioDecoder> decoder_p, Time bufferedTime, bool looped_p = false);
  ~AsyncDecoder();

  bool get(std::span<sample_t> out) override;
  void seek(uint64_t position) override;
  void setLooped(bool looped_p) override;

  uint64_t getLength() override;
  Frequency getSampleRate() override;
  uint64_t getPosition() override;
  uint64_t getLoopStart() override;
  uint64_t getLoopEnd() override;

  bool errorOccured() const override;
  FrameFormat getFormat() const override;

  // no copyable or movable
  AsyncDecoder(AsyncDecoder& oth) = delete;
  AsyncDecoder& operator= (AsyncDecoder& oth) = delete;

private:
  std::unique_ptr<AudioDecoder> decoder;
  Tools::ReaderWriterQueue<sample_t> buffer;
  std::atomic_bool run{false};
  std::atomic_bool ready{false};

  // these won't change, no need for atomic
  uint32_t length = 0;
  Frequency sampleRate;
  FrameFormat format;
  uint32_t loopStart = 0;
  uint32_t loopEnd = 0;

  std::atomic_bool ended{false};
  std::atomic_bool error{false};

  std::atomic_uint32_t position = 0;

  std::atomic_uint32_t seekPosition = 0;
  std::atomic_bool askSeek{false};

  std::atomic_bool looped{false};
  std::atomic_bool askSetLooped{false};

  std::thread thread;

  void asyncThread();
};

ResultValue<SoundBuffer> decodeSound(AudioDecoder& decoder);

class FileInput : public AudioInput {
public:
enum : uint32_t {
  PlayingID,
  LoopedID,
  PositionID,
  TempoID,
  GetPositionID,
  GetLengthID,
  FadeOutID
};
struct Parameters {
  bool looped = false;
  Time position;
  double tempo = 1.;
  bool async = false;
  
  Parameters(bool looped_p = false, Time position_p = Time::seconds(0), double tempo_p = 1., bool async_p = false);
};

  FileInput(std::unique_ptr<AudioDecoder> decoder_p, const Parameters& parameters);
  void get(std::span<sample_t> out) override;
  void setSampleRate(Frequency sampleRate) override;
  void setParameter(size_t id, ParameterValue value) override;
  ParameterValue getOutputValue(size_t id) const override;
  bool errorOccured() const override;
  bool isPlaying() const override;
  FrameFormat getFormat() const override;
private:
  Frequency sampleRate;
  std::unique_ptr<AudioDecoder> decoder;
  std::vector<Tools::SampleRateConverter> sampleRateConverters;
  bool playing = true;
  bool ended = false;
  bool looped = false;
  double tempo = 1.;

  bool fadingOut = false;  
  Tools::Smoother<Volume> volume;
};


} // namespace ZAudio