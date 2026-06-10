#pragma once

#include <SDL3/SDL.h>

#include <ZAudio/AudioInput.h>
#include <ZAudio/AudioOutput.h>
#include <ZAudio/SampleRateConversion.h>
#include <ZAudio/CallbackIO.h>

namespace ZAudio {


class SDL_Output : public AudioOutput {
public:
  SDL_Output(FrameFormat format_p, Frequency inSampleRate_p, SDL_AudioStream* stream_p, std::shared_ptr<std::atomic_bool> stopFlag_p, std::shared_ptr<std::atomic_bool> finishedFlag_p) :
    format(format_p),
    inSampleRate(inSampleRate_p),
    buffer(128),
    stream(stream_p),
    stopFlag(stopFlag_p),
    finishedFlag(finishedFlag_p) {}    

  void send(std::span<const sample_t> in) override {
    finishedFlag->store(false);

    if(stopFlag->load()) {
      finishedFlag->store(true);
      return;
    }

    SDL_AudioSpec spec;
    int sampleFrames = 0;
    if(!SDL_GetAudioDeviceFormat(SDL_GetAudioStreamDevice(stream), &spec, &sampleFrames)) {
      error = true;
    }

    const int32_t hardwareBufferBytes = sampleFrames * SDL_AUDIO_FRAMESIZE(spec);
    const int32_t localBufferBytes = static_cast<int32_t>(buffer.size() * sizeof(float));
    const int32_t threshold = hardwareBufferBytes + localBufferBytes * 2;

    for(size_t i = 0; i < Tools::numberOfChannels(format); i++) {
      auto v = in[i];
      buffer[curr++] = v;
          
      if(curr == static_cast<int32_t>(buffer.size())) {
        curr = 0;

        const int32_t specFreq = spec.freq > 0 ? spec.freq : 44100;
        const int32_t specChannels = spec.channels > 0 ? spec.channels : 2;

        const std::chrono::duration<double> bufferDuration(
          static_cast<double>(sampleFrames) / (specChannels * specFreq)
        );
              
        const std::chrono::duration<double> timeout = std::max(
          bufferDuration * 2.0,
          std::chrono::duration<double>(0.02)
        );

        const auto waitStart = std::chrono::steady_clock::now();
        bool timedOut = false;

        while(SDL_GetAudioStreamQueued(stream) > threshold && !stopFlag->load()) {
          const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - waitStart;

          if(elapsed >= timeout) {
            timedOut = true;
            break;
          }

          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if(!timedOut) {
          if(!SDL_PutAudioStreamData(stream, buffer.data(), buffer.size() * sizeof(float))) {
            error = true;
          }
        }
      }
    }

    finishedFlag->store(true);
  }

  void setSampleRate(Frequency sampleRate) override {
    if(sampleRate != inSampleRate) {
      error = true;
    }
  }

  bool errorOccured() const override {
    return error;
  }

  bool ended() const override {
    return stopFlag->load();
  }

  FrameFormat getFormat() const override {
    return format;
  }

  // only move constructors
  SDL_Output(const SDL_Output& oth) = delete;
  SDL_Output(SDL_Output&& oth) = default;
  SDL_Output& operator = (const SDL_Output& oth) = delete;
  SDL_Output& operator = (SDL_Output&& oth) = default;

private:
  FrameFormat format;
  Frequency inSampleRate;

  int32_t curr = 0;
  std::vector<float> buffer;

  bool error = false;

  SDL_AudioStream* stream = nullptr;
  std::shared_ptr<std::atomic_bool> stopFlag;
  std::shared_ptr<std::atomic_bool> finishedFlag;
};

class SDL_Input : public AudioInput {
public:
  SDL_Input(FrameFormat format_p, Frequency inSampleRate_p, SDL_AudioStream* stream_p, std::shared_ptr<std::atomic_bool> stopFlag_p, std::shared_ptr<std::atomic_bool> finishedFlag_p) :
    format(format_p),
    inSampleRate(inSampleRate_p),
    buffer(128),
    stream(stream_p),
    stopFlag(stopFlag_p),
    finishedFlag(finishedFlag_p) {}

  void get(std::span<sample_t> out) override {
    finishedFlag->store(false);

    if(stopFlag->load()) {
      finishedFlag->store(true);
      return;
    }
    
    for(size_t i = 0; i < Tools::numberOfChannels(format); i++) {

      if(curr >= n) {
        curr = 0;
        n = 0;

        while(!stopFlag->load()) {

          int32_t tmp = SDL_GetAudioStreamData(stream, buffer.data() + n, (buffer.size() - n) * sizeof(float)) / sizeof(float);
          if(tmp == -1) {
            error = true;
            return;
          }

          n += tmp;
          if(n == static_cast<int32_t>(buffer.size())) {
            break;
          }

          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
      }      
      out[i] = buffer[curr++];
    }

    finishedFlag->store(true);
  }

  void setSampleRate(Frequency sampleRate) override {
    if(sampleRate != inSampleRate) {
      error = true;
    }
  }

  bool errorOccured() const override {
    return error;
  }

  bool isPlaying() const override {
    return stopFlag->load();
  }

  FrameFormat getFormat() const override {
    return format;
  }

private:
  FrameFormat format;
  Frequency inSampleRate;

  int32_t curr = 0;
  int32_t n = 0;
  std::vector<float> buffer;

  bool error = false;

  SDL_AudioStream* stream = nullptr;
  std::shared_ptr<std::atomic_bool> stopFlag;
  std::shared_ptr<std::atomic_bool> finishedFlag;
};

class SDL_IO {
public:
  SDL_IO() = default;
  ~SDL_IO();
  SDL_IO(const SDL_IO& oth) = delete;
  SDL_IO(SDL_IO&& oth) = default;
  SDL_IO& operator= (const SDL_IO& oth) = delete;
  SDL_IO& operator= (SDL_IO&& oth) = default;

  bool init(Frequency sampleRate_p);
  std::unique_ptr<SDL_Output> createDefaultOutput(FrameFormat format);
  std::unique_ptr<SDL_Input> createDefaultInput(FrameFormat format);
  std::string getError() const;
private:
  FrameFormat inputFormat = FrameFormat::Mono;
  FrameFormat outputFormat = FrameFormat::Mono;
  Frequency sampleRate;

  std::shared_ptr<std::atomic_bool> stopFlag;
  std::shared_ptr<std::atomic_bool> finishedFlag;

  std::string error;
};


} // namespace ZAudio
