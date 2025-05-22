#include <thread>
#include <portaudio.h>
#include <ZAudio_PortAudioIO.h>
#include <cmath>
#include <iostream>

namespace ZAudio {


Result PortAudioIO::init() {
  PaError err;
  err = Pa_Initialize();  
  if(err != paNoError) {        
    return Result::error(Pa_GetErrorText(err));    
  }      

  auto deviceCount = Pa_GetDeviceCount();
  devices.resize(deviceCount);

  for(int32_t i = 0; i < deviceCount; i++) {    
    auto info = Pa_GetDeviceInfo(i);
    if(!info) {
      return Result::error("error getting device info");      
    }
    devices[i].defaultSampleRate = Frequency::Hz(info->defaultSampleRate);
    devices[i].hostApiIndex = info->hostApi;
    devices[i].maxInputChannels = info->maxInputChannels;
    devices[i].maxOutputChannels = info->maxOutputChannels;
    devices[i].name = info->name;    
  }

  auto hostApiCount = Pa_GetHostApiCount();
  hostApis.resize(hostApiCount);
  for(int32_t i = 0; i < hostApiCount; i++) {    
    auto info = Pa_GetHostApiInfo(i);
    if(!info) {
      return Result::error("error getting host api info");      
    }
    hostApis[i].defaultInputDeviceIndex = info->defaultInputDevice;
    hostApis[i].defaultOutputDeviceIndex = info->defaultOutputDevice;
    hostApis[i].id = info->type;
    hostApis[i].name = info->name;
  }
  auto di = Pa_GetDefaultInputDevice();
  if(di != paNoDevice) {
    defaultInputDevice = di;  
  }
  auto du = Pa_GetDefaultOutputDevice();
  if(du != paNoDevice) {
    defaultOutputDevice = du;
  }

  return Result::success();
}

PortAudioIO::~PortAudioIO() {
  stop();
}

Result PortAudioIO::startInputOutput(int32_t numberOfInputChannels, int32_t numberOfOutputChannels, int32_t bufferSize) {  
  if(!defaultInputDevice) {
    return Result::error("No default input device");
  }
  if(!defaultOutputDevice) {
    return Result::error("No default output device");
  }
  return startInputOutput(*defaultInputDevice, numberOfInputChannels, *defaultOutputDevice, numberOfOutputChannels, bufferSize);
}

Result PortAudioIO::startOnlyInput(int32_t numberOfChannels, int32_t bufferSize) {
  if(!defaultInputDevice) {
    return Result::error("No default input device");
  }
  return startOnlyInput(*defaultInputDevice, numberOfChannels, bufferSize);
}

Result PortAudioIO::startOnlyOutput(int32_t numberOfChannels, int32_t bufferSize) {
  if(!defaultOutputDevice) {
    return Result::error("No default output device");
  }
  return startOnlyOutput(*defaultOutputDevice, numberOfChannels, bufferSize);
}

static std::vector<double> log;

static int inputOutputCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) {    
  float* in = (float*)inputBuffer;
  float* out = (float*)outputBuffer;
  Tools::InputOutputCallbackData* data = (Tools::InputOutputCallbackData*) userData;

  data->input->inputCallback(std::span<const float>(in, in + framesPerBuffer * data->input->getNumberOfChannels()));
  data->output->outputCallback(std::span<float>(out, out + framesPerBuffer * data->output->getNumberOfChannels()));

  return paContinue;  
}

static int inputOnlyCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData ) {    
  float* in = (float*)inputBuffer;  
  Tools::InputOutputCallbackData* data = (Tools::InputOutputCallbackData*) userData;

  data->input->inputCallback(std::span<const float>(in, in + framesPerBuffer * data->input->getNumberOfChannels()));  

  return paContinue;  
}

static int outputOnlyCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData ) {      
  float* out = (float*)outputBuffer;
  Tools::InputOutputCallbackData* data = (Tools::InputOutputCallbackData*) userData;

  data->output->outputCallback(std::span<float>(out, out + framesPerBuffer * data->output->getNumberOfChannels()));

  return paContinue;  
}

Result PortAudioIO::startInputOutput(int32_t inputDeviceIndex, int32_t numberOfInputChannels, int32_t outputDeviceIndex, int32_t numberOfOutputChannels, int32_t bufferSize) {  
  assert(numberOfInputChannels <= 2);
  assert(numberOfOutputChannels <= 2);
  inputFormat = (numberOfInputChannels == 2 ? FrameFormat::Stereo : FrameFormat::Mono);
  outputFormat = (numberOfOutputChannels == 2 ? FrameFormat::Stereo : FrameFormat::Mono);
  sampleRate = devices[outputDeviceIndex].defaultSampleRate;

  active = true;
  PaStreamParameters inputParameters = {      
    .device = inputDeviceIndex,
    .channelCount = numberOfInputChannels,
    .sampleFormat = paFloat32,
    .suggestedLatency = Pa_GetDeviceInfo(inputDeviceIndex)->defaultHighInputLatency,
    .hostApiSpecificStreamInfo = NULL,
  };  
  PaStreamParameters outputParameters = {      
    .device = outputDeviceIndex,
    .channelCount = numberOfOutputChannels,
    .sampleFormat = paFloat32,
    .suggestedLatency = Pa_GetDeviceInfo(outputDeviceIndex)->defaultHighInputLatency,
    .hostApiSpecificStreamInfo = NULL,
  };
  auto err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, sampleRate.Hz(), bufferSize, 0, inputOutputCallback, &data);
  if(err != paNoError) {
    return Result::error(Pa_GetErrorText(err));
  }  

  err = Pa_StartStream(stream);
  if(err != paNoError) {
    return Result::error(Pa_GetErrorText(err));
  }    
  data.input->init(numberOfInputChannels, bufferSize * numberOfInputChannels);
  data.output->init(numberOfOutputChannels, bufferSize * numberOfOutputChannels);

  return Result::success();
}

Result PortAudioIO::startOnlyInput(int32_t deviceIndex, int32_t numberOfChannels, int32_t bufferSize) {
  assert(numberOfChannels <= 2);  
  inputFormat = (numberOfChannels == 2 ? FrameFormat::Stereo : FrameFormat::Mono);  
  sampleRate = devices[deviceIndex].defaultSampleRate;

  active = true;
  PaStreamParameters inputParameters = {      
    .device = deviceIndex,
    .channelCount = numberOfChannels,
    .sampleFormat = paFloat32,
    .suggestedLatency = Pa_GetDeviceInfo(deviceIndex)->defaultHighInputLatency,
    .hostApiSpecificStreamInfo = NULL,
  };
  auto err = Pa_OpenStream(&stream, &inputParameters, nullptr, devices[deviceIndex].defaultSampleRate.Hz(), bufferSize, 0, inputOnlyCallback, &data);
  if(err != paNoError) {
    return Result::error(Pa_GetErrorText(err));
  }

  err = Pa_StartStream(stream);
  if(err != paNoError) {
    return Result::error(Pa_GetErrorText(err));
  }

  data.input->init(numberOfChannels, bufferSize * numberOfChannels);
  return Result::success();
}

Result PortAudioIO::startOnlyOutput(int32_t deviceIndex, int32_t numberOfChannels, int32_t bufferSize) {
  assert(numberOfChannels <= 2);  
  outputFormat = (numberOfChannels == 2 ? FrameFormat::Stereo : FrameFormat::Mono);  
  sampleRate = devices[deviceIndex].defaultSampleRate;

  active = true;
  PaStreamParameters outputParameters = {      
    .device = deviceIndex,
    .channelCount = numberOfChannels,
    .sampleFormat = paFloat32,
    .suggestedLatency = Pa_GetDeviceInfo(deviceIndex)->defaultHighInputLatency,
    .hostApiSpecificStreamInfo = NULL,
  };
  auto err = Pa_OpenStream(&stream, nullptr, &outputParameters, devices[deviceIndex].defaultSampleRate.Hz(), bufferSize, 0, outputOnlyCallback, &data);
  if(err != paNoError) {
    return Result::error(Pa_GetErrorText(err));
  }

  err = Pa_StartStream(stream);
  if(err != paNoError) {
    return Result::error(Pa_GetErrorText(err));
  }
  
  data.output->init(numberOfChannels, bufferSize * numberOfChannels);
  return Result::success();
}

void PortAudioIO::stop() {
  data.input->setEnded();
  data.output->setEnded();
  if(active) {
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    stream = nullptr;    
  }  
  active = false;
}

const std::vector<PortAudioIO::HostApi>& PortAudioIO::getHostApis() const {
  return hostApis;
}

const std::vector<PortAudioIO::Device>& PortAudioIO::getDevices() const {
  return devices;
}

std::optional<int32_t> PortAudioIO::getDefaultInputDevice() const {
  return defaultInputDevice;
}

std::optional<int32_t> PortAudioIO::getDefaultOutputDevice() const {
  return defaultOutputDevice;
}


} // namespace ZAudio