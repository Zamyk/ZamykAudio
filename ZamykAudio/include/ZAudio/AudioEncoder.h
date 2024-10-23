#pragma once

#include <thread>
#include <ZAudio/AudioOutput.h>
#include <ZAudio/SoundBuffer.h>
#include <ZAudio/SampleRateConversion.h>
#include <ZAudio/ReaderWriterQueue.h>

namespace ZAudio {


class AudioEncoder {
public:
  virtual ~AudioEncoder() = default;  
  virtual void send(std::span<const sample_t> out) = 0;  
  virtual Frequency getSampleRate() const = 0;
  virtual FrameFormat getFormat() const = 0;
  virtual bool errorOccured() const = 0;
  virtual bool ended() const = 0;
};

class AsyncEncoder : public AudioEncoder {
public:
  AsyncEncoder(std::unique_ptr<AudioEncoder> encoder_p, Time bufferedTime);
  ~AsyncEncoder();
  void send(std::span<const sample_t> out) override;
  Frequency getSampleRate() const override;
  FrameFormat getFormat() const override;
  bool errorOccured() const override;
  bool ended() const override;
private:
  std::unique_ptr<AudioEncoder> encoder;
  Tools::ReaderWriterQueue<sample_t> buffer;
  std::atomic_bool run{false};
  std::atomic_bool ready{false};

  // these won't change, no need for atomic
  Frequency sampleRate;
  FrameFormat format;

  std::atomic_bool ended_{false};
  std::atomic_bool error{false};

  std::thread thread;
  void asyncThread();
};

Result encodeSound(AudioEncoder& encoder, const SoundBuffer& soundBuffer);

class FileOutput : public AudioOutput {
public:
enum : uint32_t {
  StopID
};
  FileOutput(std::unique_ptr<AudioEncoder> encoder_p);
  void send(std::span<const sample_t> in) override;
  void setSampleRate(Frequency sampleRate) override;
  void setParameter(size_t id, ParameterValue value) override;
  ParameterValue getOutputValue(size_t id) override;
  bool errorOccured() const override;
  bool ended() const override;
  FrameFormat getFormat() const override;  
private:
  std::unique_ptr<AudioEncoder> encoder;    
  Frequency sampleRate;
  std::vector<Tools::SampleRateConverter> sampleRateConverters;
  bool stop = false;
};


} // namespace ZAudio