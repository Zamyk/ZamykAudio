#include <ZAudio/SDL_IO.h>

namespace ZAudio {


SDL_IO::~SDL_IO() {
  stopFlag->store(true);
  while(!finishedFlag->load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool SDL_IO::init(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  stopFlag = std::make_shared<std::atomic_bool>(false);
  finishedFlag = std::make_shared<std::atomic_bool>(false);
  
  if(!SDL_Init(SDL_INIT_AUDIO)) {
    error = SDL_GetError();
    return false;
  }  
  return true;
}

std::unique_ptr<SDL_Output> SDL_IO::createDefaultOutput(FrameFormat format) {
  SDL_AudioSpec spec = { SDL_AUDIO_F32,  static_cast<int32_t>(ZAudio::Tools::numberOfChannels(format)), static_cast<int32_t>(sampleRate.Hz()) };
  SDL_AudioStream* outputStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    
  if(!outputStream) {
    error = SDL_GetError();
    return nullptr;
  }
  if(!SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(outputStream))) {
    error = SDL_GetError();
    return nullptr;
  }    
  return std::make_unique<SDL_Output>(format, sampleRate, outputStream, stopFlag, finishedFlag);
}

std::unique_ptr<SDL_Input> SDL_IO::createDefaultInput(FrameFormat format) {
  SDL_AudioSpec spec = { SDL_AUDIO_F32, static_cast<int32_t>(ZAudio::Tools::numberOfChannels(format)), static_cast<int32_t>(sampleRate.Hz()) };
  SDL_AudioStream* inputStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);

  if(!inputStream) {
    error = SDL_GetError();
    return nullptr;
  }
  if(!SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(inputStream))) {
    error = SDL_GetError();
    return nullptr;
  }    
  return std::make_unique<SDL_Input>(format, sampleRate, inputStream, stopFlag, finishedFlag);
}

std::string SDL_IO::getError() const {
  return error;
}

} // namespace ZAudio