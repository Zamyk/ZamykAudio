#include <chrono>
#include <thread>

#include <ZAudio/EffectsInclude.h>
#include <ZAudio/ZAudioInclude.h>
#include <ZAudio/SDL_IO.h>
#include <ZAudio/FileIO.h>
#include <ZAudio_PortAudioIO.h>

using namespace ZAudio;


// for more convinient file playback see effect playback, which uses SoundCache
void filePlayback() {
  // prepare engine
  const Frequency sampleRate = Frequency::Hz(44100);
  AudioEngine engine(sampleRate);
  auto mixer = engine.addMixer(FrameFormat::Stereo);

  // prepare file input
  auto mp3 = Mp3Decoder::load("sound.mp3");
  if(!mp3) {
    std::cout << mp3.getDescription();
    return;
  }

  auto input = engine.addInput<FileInput>(std::make_unique<AsyncDecoder>(std::move(mp3.get()), Time::seconds(10)), true);

  std::cout << "Sound length is " << engine.getOutputValue(input, FileInput::GetLengthID).getTime().seconds() << " seconds" << std::endl;

  // prepare sdl output
  SDL_IO sdl;

  if(!sdl.init(sampleRate)) {
    std::cout << sdl.getError();
    return;
  }

  auto sdlOut = sdl.createDefaultOutput(FrameFormat::Stereo);
  if(!sdlOut) {
    std::cout << sdl.getError();
    return;
  }

  auto output = engine.addOutput(std::move(sdlOut));

  // play
  engine.addMixerOutput(mixer, output);
  engine.play(mixer, input);
  while(engine.isPlaying(input)) {
    std::cout << "playing at: " << engine.getOutputValue(input, FileInput::GetPositionID).getTime().seconds() << " seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void monitoringSDL() {
  // prepare engine
  const Frequency sampleRate = Frequency::Hz(44100);
  AudioEngine engine(sampleRate);
  auto mixer = engine.addMixer(FrameFormat::Stereo);

  // prepare sdl input and output
  SDL_IO sdl;

  if(!sdl.init(sampleRate)) {
    std::cout << sdl.getError();
    return;
  }

  auto sdlIn = sdl.createDefaultInput(FrameFormat::Mono);
  if(!sdlIn) {
    std::cout << sdl.getError();
    return;
  }
  auto input = engine.addInput(std::move(sdlIn));

  auto sdlOut = sdl.createDefaultOutput(FrameFormat::Stereo);
  if(!sdlOut) {
    std::cout << sdl.getError();
    return;
  }
  auto output = engine.addOutput(std::move(sdlOut));




  // play
  engine.addMixerOutput(mixer, output);


  std::cout << "Do you want some delay?(Y:n)";
  char ans;
  std::cin >> ans;

  if(tolower(ans) == 'y') {
    DelayEffect::Parameters delayParameters;
    delayParameters.delayTime = Time::seconds(0.5);
    engine.play(mixer, input, engine.addEffect<DelayEffect>(delayParameters));
  }
  else {
    engine.play(mixer, input);
  }

  std::cout << "Type anything to end" << std::endl;
  std::string str;
  std::cin >> str;
}

void monitoringPortAudio() {
  // prepare engine
  const Frequency sampleRate = Frequency::Hz(44100);
  AudioEngine engine(sampleRate);
  auto mixer = engine.addMixer(FrameFormat::Stereo);

  // prepare port audio input and output
  PortAudioIO portAudioIO;
  Result res = portAudioIO.init();

  if(!res) {
    std::cout << res.getDescription();
    return;
  }

  {
    auto devices = portAudioIO.getDevices();
    int i = 0;
    for(auto& dev : devices) {
      std::cout << "(" << i << ") " << dev.name << " max input channels: " << dev.maxInputChannels << " max output channels: " << dev.maxOutputChannels << std::endl;
      i++;
    }
  }

  std::cout << "Default input: " << *portAudioIO.getDefaultInputDevice() << std::endl;
  std::cout << "Default output: " << *portAudioIO.getDefaultOutputDevice() << std::endl;

  std::cout << "Choose input device (-1 == default)" << std::endl;
  int inputDeviceIndex = 0;
  std::cin >> inputDeviceIndex;

  std::cout << "Choose output device (-1 == default)" << std::endl;
  int outputDeviceIndex = 0;
  std::cin >> outputDeviceIndex;

  if(inputDeviceIndex == -1) {
    inputDeviceIndex = *portAudioIO.getDefaultInputDevice();
  }
  if(outputDeviceIndex == -1) {
    outputDeviceIndex = *portAudioIO.getDefaultOutputDevice();
  }

  res = portAudioIO.startInputOutput(inputDeviceIndex, 1, outputDeviceIndex, 1, 256);
  if(!res) {
    std::cout << res.getDescription();
    return;
  }
  auto input = engine.addInput(portAudioIO.getAudioInput());
  auto output = engine.addOutput(portAudioIO.getAudioOutput());

  std::cout << "Do you want vibrato?(Y:n)";
  char ans;
  std::cin >> ans;

  engine.addMixerOutput(mixer, output);

  if(tolower(ans) == 'y') {
    VibratoEffect::Parameters vibratoParameters;
    vibratoParameters.depth = 1;
    vibratoParameters.maxDelay = Time::miliseconds(20);
    engine.play(mixer, input, engine.addEffect<VibratoEffect>(vibratoParameters));
  }
  else {
    engine.play(mixer, input);
  }

  std::cout << "Type anything to end" << std::endl;
  std::string str;
  std::cin >> str;
}

void monitoringAndRecordingToFile() {
  // prepare engine
  const Frequency sampleRate = Frequency::Hz(44100);
  AudioEngine engine(sampleRate);
  auto mixer = engine.addMixer(FrameFormat::Stereo);

  // prepare sdl input and output
  SDL_IO sdl;

  if(!sdl.init(sampleRate)) {
    std::cout << sdl.getError();
    return;
  }

  auto sdlIn = sdl.createDefaultInput(FrameFormat::Mono);
  if(!sdlIn) {
    std::cout << sdl.getError();
    return;
  }
  auto input = engine.addInput(std::move(sdlIn));

  auto sdlOut = sdl.createDefaultOutput(FrameFormat::Stereo);
  if(!sdlOut) {
    std::cout << sdl.getError();
    return;
  }
  auto output = engine.addOutput(std::move(sdlOut));


  // prepare file output (AsyncEncoder is used because normal may block other audio processing)
  auto encoder = WavEncoder::create(sampleRate, FrameFormat::Stereo, "output.wav");
  auto output2 = engine.addOutput<FileOutput>(std::make_unique<AsyncEncoder>(std::move(encoder.get()), Time::seconds(5)));

  // play
  engine.addMixerOutput(mixer, output);
  engine.addMixerOutput(mixer, output2);

  std::cout << "Do you want deep voice?(Y:n)";
  char ans;
  std::cin >> ans;

  if(tolower(ans) == 'y') {
    PitchShiftEffect::Parameters pitchShiftParameters;
    pitchShiftParameters.algorithm = PitchShiftEffect::Type::Normal;
    pitchShiftParameters.pitchShiftRatio = 0.7;
    engine.play(mixer, input, engine.addEffect<PitchShiftEffect>(pitchShiftParameters));
  }
  else {
    engine.play(mixer, input);
  }

  std::cout << "Type anything to end" << std::endl;
  std::string str;
  std::cin >> str;
}


void looperExample() {
  // prepare engine
  const Frequency sampleRate = Frequency::Hz(44100);
  AudioEngine engine(sampleRate);
  auto mixer = engine.addMixer(FrameFormat::Stereo);

  // prepare sdl input and output
  SDL_IO sdl;

  if(!sdl.init(sampleRate)) {
    std::cout << sdl.getError();
    return;
  }

  auto sdlIn = sdl.createDefaultInput(FrameFormat::Mono);
  if(!sdlIn) {
    std::cout << sdl.getError();
    return;
  }
  auto input = engine.addInput(std::move(sdlIn));

  auto sdlOut = sdl.createDefaultOutput(FrameFormat::Stereo);
  if(!sdlOut) {
    std::cout << sdl.getError();
    return;
  }
  auto output = engine.addOutput(std::move(sdlOut));

  // add looper
  double seconds;
  std::cout << "How long you want looper to be in seconds?" << std::endl;
  std::cin >> seconds;

  auto looper = engine.addEffect<LooperEffect>(LooperEffect::Parameters(Time::seconds(seconds), Volume::dB(0), Volume::dB(-2)));

  // play
  engine.addMixerOutput(mixer, output);

  engine.play(mixer, input, looper);

  std::cout << "r - start recording\no - overdub\nc - clear\ns - pause\np - play\nq - quit" << std::endl;

  while(true) {
    char c;
    std::cin >> c;
    switch (c) {
      case 'r':
        engine.setEffectParameter(looper, LooperEffect::ModeID, ParameterValue::fromEnum(LooperEffect::Mode::Recording));
        break;

      case 'o':
        engine.setEffectParameter(looper, LooperEffect::ModeID, ParameterValue::fromEnum(LooperEffect::Mode::Overdubbing));
        break;

      case 'c':
        engine.setEffectParameter(looper, LooperEffect::ClearID, ParameterValue::boolean(true));
        break;

      case 's':
        engine.setEffectParameter(looper, LooperEffect::ModeID, ParameterValue::fromEnum(LooperEffect::Mode::Paused));
        break;

      case 'p':
        engine.setEffectParameter(looper, LooperEffect::ModeID, ParameterValue::fromEnum(LooperEffect::Mode::Playing));
        break;

      case 'q':
        return;

      default:
        std::cout << "Wrong command\n";
    }
  }
}

void effectPlayback() {
  // prepare engine

  // every sample rate should work, but its best if its same as inputs so there is no need for conversion
  const Frequency sampleRate = Frequency::Hz(56000);
  AudioEngine engine(sampleRate);
  auto mixer = engine.addMixer(FrameFormat::Stereo);

  // prepare sdl output
  SDL_IO sdl;

  if(!sdl.init(sampleRate)) {
    std::cout << sdl.getError();
    return;
  }

  auto sdlOut = sdl.createDefaultOutput(FrameFormat::Stereo);
  if(!sdlOut) {
    std::cout << sdl.getError();
    return;
  }

  auto output = engine.addOutput(std::move(sdlOut));

  // prepare cache input
  SoundCache cache;
  cache.addLoadingFunction(".wav", WavDecoder::load);
  cache.addLoadingFunction(".flac",
    [](const std::filesystem::path& path) -> ResultValue<std::unique_ptr<AudioDecoder>> {
      auto ans = FlacDecoder::load(path);
      if(ans) {
        return static_cast<std::unique_ptr<AudioDecoder>>(std::move(ans.get()));
      }
      else {
        return Result::error(ans.getDescription());
      }
    }
  );
  cache.addLoadingFunction(".mp3", Mp3Decoder::load);

  auto soundID = cache.add("sound.mp3");

  if(!soundID) {
    std::cout << cache.getError();
    return;
  }

  auto cacheInput = cache.getSound(*soundID);
  if(!cacheInput) {
    std::cout << cache.getError();
    return;
  }
  auto input = engine.addInput(std::move(cacheInput));


  // prepare effect
  std::cout << "Please give path to effect(.xml)" << std::endl;
  std::filesystem::path path;
  std::cin >> path;
  if(path.extension() != ".xml") {
    std::cout << "Wrong extension!";
    return;
  }

  std::ifstream in(path);
  auto effect = loadEffectFromXML(in);
  if(!effect) {
    std::cout << effect.getDescription();
    return;
  }

  // play
  engine.addMixerOutput(mixer, output);
  engine.play(mixer, input, engine.addEffect(std::move(effect.get())));
  while(engine.isPlaying(input)) {
    std::cout << "playing at: " << engine.getOutputValue(input, FileInput::GetPositionID).getTime().seconds() << " seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

int main() {
  filePlayback();
  return 0;
}