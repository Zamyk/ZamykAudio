#pragma once

#include <functional>
#include <utility>
#include <filesystem>
#include <memory>

#include <ZAudio/CommonTypes.h>

#include <ZAudio/AudioEncoder.h>
#include <ZAudio/AudioDecoder.h>

#include <dr_flac.h>
#include <dr_mp3.h>
#include <dr_wav.h>

namespace ZAudio {


class FileInputStream {
public:
enum struct SeekOrigin {
  Start, Current
};
  virtual size_t read(uint8_t* data, size_t n) = 0;
  virtual bool seek(size_t pos, SeekOrigin origin) = 0;
  virtual ~FileInputStream() {}
};

class FileOutputStream {
public:
enum struct SeekOrigin {
  Start, Current
};
  virtual size_t write(const uint8_t* data, size_t n) = 0;
  virtual bool seek(size_t pos, SeekOrigin origin) = 0;
  virtual ~FileOutputStream() {}
private:
};

class WavDecoder : public AudioDecoder {
public:
  static ResultValue<std::unique_ptr<AudioDecoder>> load(const std::filesystem::path& path);
  static ResultValue<std::unique_ptr<AudioDecoder>> loadFromStream(std::unique_ptr<FileInputStream> fileInput);
  WavDecoder(std::unique_ptr<FileInputStream> fileInput, Result& result);
  ~WavDecoder();

  // move only
  WavDecoder(const WavDecoder& oth) = delete;
  WavDecoder(WavDecoder&& oth) = default;
  WavDecoder& operator= (const WavDecoder& oth) = delete;
  WavDecoder& operator= (WavDecoder&& oth) = default;

  bool get(std::span<sample_t> out) override;
  void seek(uint64_t position) override;
  void setLooped(bool looped_p) override;

  uint64_t getLength() override;
  Frequency getSampleRate() override;

  uint64_t getPosition() override;
  bool errorOccured() const override;
  FrameFormat getFormat() const override;
private:
  bool looped = false;
  std::unique_ptr<FileInputStream> fileInput;
  bool init = false;
  drwav wav;
  FrameFormat format;
  bool error = false;
};

class FlacDecoder : public AudioDecoder {
public:
struct FlacFileInput {
  std::unique_ptr<FileInputStream> input;
  int64_t loopStart = -1;
  int64_t loopEnd = -1;

  FlacFileInput(std::unique_ptr<FileInputStream> input_p) : input(std::move(input_p)) {}
};
  static ResultValue<std::unique_ptr<AudioDecoder>> load(const std::filesystem::path& path, bool loadLoops = false);
  static ResultValue<std::unique_ptr<AudioDecoder>> loadFromStream(std::unique_ptr<FileInputStream> fileInput, bool loadLoops = false);

  FlacDecoder(std::unique_ptr<FileInputStream> fileInput, bool loadLoops, Result& result);
  ~FlacDecoder();

  // move only
  FlacDecoder(const FlacDecoder& oth) = delete;
  FlacDecoder(FlacDecoder&& oth) = default;
  FlacDecoder& operator= (const FlacDecoder& oth) = delete;
  FlacDecoder& operator= (FlacDecoder&& oth) = default;

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
private:
  bool looped = false;
  std::unique_ptr<FlacFileInput> fileInput;
  drflac* flac;
  FrameFormat format;
  bool error = false;
  uint32_t loopStart = 0;
  uint32_t loopEnd = 0;
};

class Mp3Decoder : public AudioDecoder {
public:
  static ResultValue<std::unique_ptr<AudioDecoder>> load(const std::filesystem::path& path);
  static ResultValue<std::unique_ptr<AudioDecoder>> loadFromStream(std::unique_ptr<FileInputStream> fileInput);

  Mp3Decoder(std::unique_ptr<FileInputStream> fileInput_p, Result& result);

  ~Mp3Decoder();

  // move only
  Mp3Decoder(const Mp3Decoder& oth) = delete;
  Mp3Decoder(Mp3Decoder&& oth) = default;
  Mp3Decoder& operator= (const Mp3Decoder& oth) = delete;
  Mp3Decoder& operator= (Mp3Decoder&& oth) = default;

  bool get(std::span<sample_t> out) override;
  void seek(uint64_t position) override;
  void setLooped(bool looped_p) override;

  uint64_t getLength() override;
  Frequency getSampleRate() override;
  uint64_t getPosition() override;

  bool errorOccured() const override;
  FrameFormat getFormat() const override;
private:
  bool looped = false;
  std::unique_ptr<FileInputStream> fileInput;
  bool init = false;
  drmp3 mp3;
  FrameFormat format;
  bool error = false;
};

class WavEncoder : public AudioEncoder {
public:
  static ResultValue<std::unique_ptr<AudioEncoder>> create(Frequency sampleRate_p, FrameFormat format_p, const std::filesystem::path& path_p);
  static ResultValue<std::unique_ptr<AudioEncoder>> createWithStream(Frequency sampleRate_p, FrameFormat format_p, std::unique_ptr<FileOutputStream> fileOutput);

  WavEncoder(Frequency sampleRate_p, FrameFormat format_p, const std::filesystem::path& path_p, Result& result);
  WavEncoder(Frequency sampleRate_p, FrameFormat format_p, std::unique_ptr<FileOutputStream> fileOutput_p, Result& result);
  ~WavEncoder();

  // move only
  WavEncoder(const WavEncoder& oth) = delete;
  WavEncoder(WavEncoder&& oth) = default;
  WavEncoder& operator= (const WavEncoder& oth) = delete;
  WavEncoder& operator= (WavEncoder&& oth) = default;

  Frequency getSampleRate() const override;
  FrameFormat getFormat() const override;
  void send(std::span<const sample_t> out) override;

  void setLooped(bool looped_p);

  bool errorOccured() const override;
  bool ended() const override;
private:
  std::unique_ptr<FileOutputStream> fileOutput;
  Frequency sampleRate;
  FrameFormat format;
  drwav wav;
  bool error = false;
};


} // namespace ZAudio