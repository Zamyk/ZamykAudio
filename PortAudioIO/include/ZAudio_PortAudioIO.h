#pragma once

#include <atomic>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/AudioInput.h>
#include <ZAudio/AudioOutput.h>
#include <ZAudio/SampleRateConversion.h>
#include <ZAudio/CallbackIO.h>

typedef void PaStream; // forward

namespace ZAudio {

using PortAudioInput = Tools::CallbackInput;
using PortAudioOutput = Tools::CallbackOutput;

class PortAudioIO {
public:
struct HostApi {
  int32_t id = 0;  
  std::string name;
  int32_t defaultInputDeviceIndex = 0;
  int32_t defaultOutputDeviceIndex = 0;
};
struct Device {
  std::string name;
  int32_t hostApiIndex = 0;
  int32_t maxInputChannels = 0;
  int32_t maxOutputChannels = 0;
  Frequency defaultSampleRate;  
};  
  ~PortAudioIO();

  Result init();
  Result startInputOutput(int32_t numberOfInputChannels_p, int32_t numberOfOutputChannels_p, int32_t bufferSize);  
  Result startOnlyInput(int32_t numberOfChannels, int32_t bufferSize);
  Result startOnlyOutput(int32_t numberOfChannels, int32_t bufferSize);

  Result startInputOutput(int32_t inputDeviceIndex, int32_t numberOfInputChannels_p, int32_t outputDeviceIndex, int32_t numberOfOutputChannels_p, int32_t bufferSize);
  Result startOnlyInput(int32_t deviceIndex, int32_t numberOfChannels, int32_t bufferSize);
  Result startOnlyOutput(int32_t deviceIndex, int32_t numberOfChannels, int32_t bufferSize);
  void stop();

  const std::vector<HostApi>& getHostApis() const;
  const std::vector<Device>& getDevices() const;

  std::optional<int32_t> getDefaultInputDevice() const;
  std::optional<int32_t> getDefaultOutputDevice() const;

  std::unique_ptr<PortAudioInput> getAudioInput(bool blocking = true) {
    return std::make_unique<PortAudioInput>(inputFormat, sampleRate, data.input, blocking);
  }

  std::unique_ptr<PortAudioOutput> getAudioOutput(bool blocking = true) {
    return std::make_unique<PortAudioOutput>(outputFormat, sampleRate, data.output, blocking);
  }
  
private:  
  Tools::InputOutputCallbackData data;
  std::vector<HostApi> hostApis;
  std::vector<Device> devices;
  std::optional<int32_t> defaultInputDevice = 0;
  std::optional<int32_t> defaultOutputDevice = 0;
  bool active = false;  
  PaStream* stream = nullptr;

  FrameFormat inputFormat = FrameFormat::Mono;
  FrameFormat outputFormat = FrameFormat::Mono;  
  Frequency sampleRate;
};


} // namespace ZAudio