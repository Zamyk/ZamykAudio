
- [Documentation for ZamykAudio library](#documentation-for-zamykaudio-library)
  - [AudioEngine](#audioengine)
  - [AudioInput](#audioinput)
    - [FileInput](#fileinput)
    - [Recording:](#recording)
  - [Audio Output](#audio-output)
    - [FileOutput](#fileoutput)
    - [Speaker Output](#speaker-output)
  - [Effects](#effects)
    - [Effect](#effect)
    - [AutoWahEffect](#autowaheffect)
    - [BitCrusherEffect](#bitcrushereffect)
    - [BypassEffect](#bypasseffect)
    - [DelayEffect](#delayeffect)
    - [DuckDelayEffect](#duckdelayeffect)
    - [DynamicsProcessorEffect](#dynamicsprocessoreffect)
    - [FilterEffect](#filtereffect)
    - [FlangerEffect](#flangereffect)
    - [LfoWahEffect](#lfowaheffect)
    - [LooperEffect](#loopereffect)
    - [MonoToStereoAdapter](#monotostereoadapter)
    - [ParallelEffect](#paralleleffect)
    - [PhaserEffect](#phasereffect)
    - [PitchShiftEffect](#pitchshifteffect)
    - [PingPongDelayEffect](#pingpongdelayeffect)
    - [ReverseDelayEffect](#reversedelayeffect)
    - [RobotEffect](#roboteffect)
    - [SequenceFilterEffect](#sequencefiltereffect)
    - [SerialEffect](#serialeffect)
    - [Spatial2dEffect](#spatial2deffect)
    - [StereoChorusEffect:](#stereochoruseffect)
    - [StereoFlangerEffect:](#stereoflangereffect)
    - [StereoPhaserEffect:](#stereophasereffect)
    - [TremoloEffect](#tremoloeffect)
    - [TubePreampEffect](#tubepreampeffect)
    - [VolumeControlEffect](#volumecontroleffect)
    - [WhisperEffect](#whispereffect)
  - [Tools](#tools)
    - [AnalogFilter](#analogfilter)
    - [AudioDelay](#audiodelay)
    - [AudioDetector](#audiodetector)
    - [CallbackIO](#callbackio)
    - [CircularBuffer](#circularbuffer)
    - [DelayGainController](#delaygaincontroller)
    - [EffectSerializer](#effectserializer)
    - [FFT](#fft)
    - [FIR\_Filter](#fir_filter)
    - [PhaseShifter](#phaseshifter)
    - [ReaderWriterQueue](#readerwriterqueue)
    - [SampleRateConversion](#samplerateconversion)
    - [LowFrequencyOscillator](#lowfrequencyoscillator)
    - [ModulatedDelay](#modulateddelay)
    - [PhaseVocoder](#phasevocoder)
    - [PhaseCorrector](#phasecorrector)
    - [SimpleDelay](#simpledelay)
    - [Smoother](#smoother)
    - [StringTools](#stringtools)
    - [ThreadTools](#threadtools)
    - [TreeDatabase](#treedatabase)
    - [WaveShapers](#waveshapers)
    - [WindowFunction](#windowfunction)
  - [Examples](#examples)
  - [Building](#building)
    - [ZamykAudio](#zamykaudio)
    - [SDL\_IO](#sdl_io)
    - [PortAudioIO](#portaudioio)
    - [fftw3](#fftw3)
  - [Types](#types)
    - [sample\_t](#sample_t)
    - [FrameFormat](#frameformat)
    - [SoundBuffer](#soundbuffer)
    - [Time](#time)
    - [Volume](#volume)
    - [Frequency](#frequency)
    - [ParameterValue](#parametervalue)
    - [Result](#result)
    - [ResultValue](#resultvalue)
  - [Dependencies](#dependencies)
    - [dr\_libs](#dr_libs)
    - [PortAudio](#portaudio)
    - [SDL3](#sdl3)
    - [pugi xml](#pugi-xml)
    - [fftw](#fftw)
  - [License](#license)

# Documentation for ZamykAudio library

## AudioEngine
AudioEngine handles live playing and mixing from input to output.
- Handles thread safe setting parameters of inputs, outputs and effects
- Handles thread safe output values from inputs, outputs and effects

How to use:

Create AudioEngine, sampleRate will be used for all inputs, outputs and effects (if they operate on diffrent one, they need to handle conversion)
```cpp
AudioEngine(Frequency sampleRate_p)
```
\
All sounds need to be played through mixers, to add mixer you can either add one specifyng frame format that it will use or create one with effect,
frame format of mixer will be same as effect output frame format. Mixer gets many inputs, adds them together and porcesses with effect(if it exists).
All frame formats are automatically converted, so if you play mono sound to stereo mixer it will automatically duplicate channel.
Effect can be anything, but probably most usefull(for mixer) are DynamicsProcessor(compressor) and VolumeControl
``` cpp
MixerHandle addMixer(FrameFormat format)
MixerHandle addMixer(EffectHandle effect)
```
\
Effect of mixer can also be changed(or set if none was set at the start)
``` cpp
void setMixerEffect(const MixerHandle& mixer, const EffectHandle& effect)
```
\
To add and remove mixers outputs: (Mixer can have many outputs and also one output can be used in many mixers)
``` cpp
void addMixerOutput(const MixerHandle& mixer, const OutputHandle& output)
void removeMixerOutput(const MixerHandle& mixer, const OutputHandle& output)
```
\
To play some sound with mixer (Similliary to outputs, one input can be used in many mixers):
Effects here are individual effects of input.
``` cpp
void play(const MixerHandle& mixer, const InputHandle& input, const EffectHandle& effect) // play input to mixer with effect
void play(const MixerHandle& mixer, const InputHandle& input)                             // play input to mixer without effect
void stop(const MixerHandle& mixer, const InputHandle& input)                             // stop input
```
\
Before adding input, output or effect their handles need to be optained. There are 2 possibilities, move unique_ptr to engine, or create it directly
when inputs/outputs/effects are added, engine will automatically call setSampleRate
```cpp
EffectHandle addEffect(std::unique_ptr<Effect> effect)

template<typename T, typename... Args>
EffectHandle addEffect(Args&&... args)

InputHandle addInput(std::unique_ptr<AudioInput> input)

template<typename T, typename... Args>
InputHandle addInput(Args&&... args)

OutputHandle addOutput(std::unique_ptr<AudioOutput> output)

template<typename T, typename... Args>
OutputHandle addOutput(Args&&... args)

// example (with effect, but works the same for input and output):

DelayEffect::Parameters parameters = ...;
auto effect = engine.addEffect<DelayEffect>(parameters);

// or

auto effect = engine.addEffect(std::make_unique<DelayEffect>(parameters));
```
\
after adding effect/input/output to engine, there is no direct access to them, which is good, because they are working in another thread.
To setParameter of effect/input/output these methods can be used:

```cpp
void setEffectParameter(const EffectHandle& handle, size_t parameterID, const ParameterValue& v)
void setMultiEffectParameter(const EffectHandle& handle, size_t effectID, size_t parameterID, const ParameterValue& v)
void setInputParameter(const InputHandle& handle, size_t parameterID, const ParameterValue& v )
void setOutputParameter(const OutputHandle& handle, size_t parameterID, const ParameterValue& v)

// example:

VolumeControlEffect::Parameters parameters;
auto effect = engine.addEffect<VolumeControlEffect>(parameters);
engine.setEffectParameter(effect, VolumeControlEffect::VolumeChangeID, Volume::dB(-6));
```
\
Simiiliary there are also some methods, for getting some information abut inputs/outputs and effects:
```cpp
bool isPlaying(const InputHandle& handle) // returns true if input is currently playing something (for input it doesn't mean it won't ever play again)
bool hasEnded(const OutputHandle& handle) // returns true if output has ended, it means that it won't every play again
ParameterValue getOutputValue(const InputHandle& handle, size_t id)  // returns some output value of input (for example position in sound file)
ParameterValue getOutputValue(const OutputHandle& handle, size_t id)
ParameterValue getOutputValue(const EffectHandle& handle, size_t id)

//example - prints current position in file every 16 miliseconds

// ... open some file
auto in = engine.addInput<FileInput> // ...

// ... play it via some mixer, add some output

while(engine.isPlaying(in)) {
  std::this_thread::sleep_for(std::chrono::seconds(0.3));
  std::cout << engine.getOutputValue(input, ZAudio::FileInput::GetPositionID).getTime().seconds() << " seconds" << std::endl;
}
```
---
(nearly) full example (there are many, full examples examples.cpp):

```cpp
int main() {
  using namespace ZAudio;
  const Frequency sampleRate = Frequency::Hz(44100);
  AudioEngine enigne(sampleRate);

  // ... open mp3
  auto in = engine.addInput<FileInput> //...

  // ... open output
  auto out = engine.addOutput //...

  ZAudio::DynamicsProcessorEffect::Parameters parameters;
  parameters.type = ZAudio::DynamicsProcessorEffect::Type::Compressor;
  parameters.ratio = 1;
  parameters.threshold = ZAudio::Volume::dB(-10);

  auto effect = engine.addEffect<ZAudio::DynamicsProcessorEffect>(parameters);

  auto mixer = engine.addMixer(effect);

  engine.addMixerOutput(mixer, out);
  engine.play(mixer, in);
  while(engine.isPlaying(mixer, in)) {
    // wait
  }
}
```

## AudioInput
AudioInput is interface, that represents any input (for example from file, microphone, etc.). It is important to note, that AudioEngine (or user if used without AudioEngine), needs to call setSampleRate
before using it. AudioEngine does it automatically, and if sample rate of input is diffrent than set, AudioInput must convert it itself.

```cpp
class AudioInput {
public:
  virtual ~AudioInput() = default;
  // fill out frame with samples
  virtual void get(std::span<sample_t> out) = 0;

  virtual void setSampleRate(Frequency sampleRate) = 0;

  // set parameter(no need ot override if there aren't any parameters to set)
  virtual void setParameter(size_t id, ParameterValue value) {}

  // get some output value (no need ot override if there aren't any values to get)
  virtual ParameterValue getOutputValue(size_t id) const { return ParameterValue(); }

  // return true if some error occured
  virtual bool errorOccured() const = 0;

  // returns true if is currently playing or will play in future without any action from outside, false if it won't play anything ever or unitl setParameter is called
  virtual bool isPlaying() const = 0;

  // returns output format
  virtual FrameFormat getFormat() const = 0;
};
```
\
To make own AudioInput just derive from from AudioInput and override = 0 methods.
### FileInput
FileInput class is used to play from any AudioDecoder, most likely these will be sound files or buffers.

- FileInput possible actions
```cpp
enum : uint32_t {
  PlayingID,           // call setParameter(PlayingID, bool v)  to set playing status
  LoopedID,            // call setParameter(LoopedID, bool v)   to set loop status
  PositionID,          // call setParameter(PositionID, Time v) to seek in file
  TempoID,             // call setParameter(TempoID, double v)  to set playback tempo
  GetPositionID,
  GetLength, 
  FadeOutID            // call setParameter(FadeOutID, Time v)  to make input fade out for Time seconds and then end
};
```
- api of FileInput
```cpp


struct Parameters {
  bool looped = false; // looped true - sound will be looped,
  Time position;       // position - starting position in sound (for example position = 10s skips first ten seconds of sound)
  double tempo = 1.;   // how fast sound will be played, for example 2.0 means twice as fast, 0.5 means two times slower
  bool async = false;  // async true - Decoder will be used from another thread(good for files bad for buffer)
};

FileInput(std::unique_ptr<AudioDecoder> decoder_p, const Parameters& parameters);
void get(std::span<sample_t> out) override;
void setSampleRate(Frequency sampleRate) override;
void setParameter(size_t id, ParameterValue value) override;
ParameterValue getOutputValue(size_t id) const override;
bool errorOccured() const override;
bool isPlaying() const override;                              // isPlaing returns false if paused, or not looped and ended
FrameFormat getFormat() const override;
```
To construct FileInput AudioDecoder is needed. It's just interface for file like inputs, without need of sample rate conversion(FileInput takes care of that).

```cpp
class AudioDecoder {
public:
  virtual ~AudioDecoder() = default;
  virtual bool get(std::span<sample_t> out) = 0; // get frame
  virtual void seek(uint64_t position) = 0;      // jump to some position in file
  virtual void setLooped(bool looped_p) = 0;     // set AudioDecoder too loop itself(true) or stop looping itself(false),
                                                 // needed here because of for example AsyncDecoder

  virtual uint64_t getLength() = 0;              // length of whole file in samples
  virtual Frequency getSampleRate() = 0;         // sample rate of file
  virtual uint64_t getPosition() = 0;            // current position in file

  virtual uint64_t getLoopStart() {              // returns loop start position in samples, normally its just 0,
    return 0;                                    // but for example FlacDecoder supports custom loop start and loop end markings
  }

  virtual uint64_t getLoopEnd() {                // returns loop end position in samples, normally its just length of file,
    return getLength();
  }

  virtual bool errorOccured() const = 0;
  virtual FrameFormat getFormat() const = 0;
};
```
---

There are decoders for .wav, .mp3 and .flac files. All these can be loaded from path or with simple interface for reading files.

- to load interface just override read and seek methods remeber to add closing to destructor
- the Mp3Decoder, WavDecoder and FlacDecoder may use the FileInputStream as long as they want - not only for constructor duration
```cpp
// Interface for loading files:
class FileInputStream {
public:
enum struct SeekOrigin {
  Start, Current
};
  virtual size_t read(uint8_t* data, size_t n) = 0;
  virtual bool seek(size_t pos, SeekOrigin origin) = 0;
  virtual ~FileInputStream() {}
};
```

---

- WavDecoder: \
To read wav file WavDecoder can be used. It provides all AudioDecoder methods.

```cpp
static ResultValue<std::unique_ptr<AudioDecoder>> WavDecoder::load(const std::filesystem::path& path);
static ResultValue<std::unique_ptr<AudioDecoder>> WavDecoder::loadFromCallback(const FileInputCallbacks& callbacks);
```
---
- Mp3Decoder: \
To read mp3 file Mp3Decoder can be used. It provides all AudioDecoder methods.
```cpp
static ResultValue<std::unique_ptr<AudioDecoder>> Mp3Decoder::load(const std::filesystem::path& path);
static ResultValue<std::unique_ptr<AudioDecoder>> Mp3Decoder::loadFromCallback(const FileInputCallbacks& callbacks);
```
---
-  FlacDecoder: \
To read flac file FlacDecoder can be used. It provides all AudioDecoder methods.
- FlacDecoder supports custom loop start and end, to make it try to load these use loadLoops = true, false is default for same behaviour as other decoders
- tutorial about adding loops in audacity and more https://wohlsoft.ru/pgewiki/How_To:_Looping_music_files 

```cpp
static FlacDecoder::CreationResult FlacDecoder::load(const std::filesystem::path& path, bool loadLoops  = false);
static FlacDecoder::CreationResult FlacDecoder::loadFromCallback(const FileInputCallbacks& callbacks, bool loadLoops = false);
// CreationResult inherits from ResultValue<std::unique_ptr<AudioDecoder>> and adds additional bool loadedLoops() method

```
---

Now to play some sound from file just create decoder and then make FileInput from it.

```cpp

void playMp3(AudioEngine& engine, MixerHandle& mixer, const std::filesystem::path& path) {
  auto mp3 = Mp3Decoder::load(path);
  if(!mp3) {
    std::cout << mp3.getDescription();
    return;
  }
  engine.play(mixer, engine.addInput<FileInput>(mp3.get()));
}

void playWav(AudioEngine& engine, MixerHandle& mixer, const std::filesystem::path& path) {
  auto wav = WavDecoder::load(path);
  if(!wav) {
    std::cout << wav.getDescription();
    return;
  }
  engine.play(mixer, engine.addInput<FileInput>(wav.get()));
}

void playFlac(AudioEngine& engine, MixerHandle& mixer, const std::filesystem::path& path) {
  auto flac = FlacDecoder::load(path);
  if(!flac) {
    std::cout << flac.getDescription();
    return;
  }
  engine.play(mixer, engine.addInput<FileInput>(flac.get()));
}

```

- For playing from memory, there is BufferDecoder class, that takes [SoundBuffer](#soundbuffer) object

and to construct BufferDecoder:

```cpp
BufferDecoder(SoundBuffer&& buffer_p);
BufferDecoder(std::shared_ptr<const SoundBuffer> buffer_p);

// example

void playFromBuffer(AudioEngine& engine, const MixerHandle& mixerHandle, SoundBuffer&& buffer) {
  engine.play(mixer, BufferDecoder(std::move(buffer)));
}

```

It is also possible to use decoders to load into SoundBuffer with decodeSound method:

```cpp
ResultValue<SoundBuffer> decodeSound(AudioDecoder& decoder);
```

There is AsyncDecoder class, that takes some other decoder and uses it async for faster no glitch stream.
```cpp
// decoder_p is decoder that will be used async, bufferTime is how long buffer should be(longer better but more memory occupied)
AsyncDecoder(std::unique_ptr<AudioDecoder> decoder_p, Time bufferedTime);
```

It is not very convinient to use paths everywhere, and check what type they are, SoundCache class makes it possible to store sounds as IDs, loading automaically looking at extensions
and stream or load to buffer before.
\
To make SoundCache handle (for exampe ".wav") you need to add loading function to it. Then just by passing path ending with provided extension it will use matching loading function.

```cpp
using LoadingFunction = std::function<ResultValue<std::unique_ptr<AudioDecoder>>(const std::filesystem::path&)>;
void addLoadingFunction(const std::string& extension, LoadingFunction loadingFunction);

// example:
soundCache.addLoadingFunction(".wav", WavDecoder::load);
```
- to read from the FileInputStream interface instead of path (for example android requires different file io in assets),
 one can use lambda expression that behaves like load from available decoders - taking path and returning 
```
ResultValue<std::unique_ptr<AudioDecoder>> 
```

Now to add some sound to Cache use add

```cpp
enum struct OpenMode {
  Stream,   // do not load sound to memory before playing, good for long sounds like music (Decoders are used through AsyncDecoder in this mode)
  PreBuffer // loads sound to memory before playing, good for short sounds that are played often
};

std::optional<CacheSoundID> add(const std::filesystem::path& path, OpenMode openMode = SoundCache::OpenMode::PreBuffer);
```

To get FileInput from SoundCache:
```cpp
std::unique_ptr<FileInput> getSound(CacheSoundID id, bool playing = true, bool looped = false, Time position = Time::seconds(0));
```

if add returns std::nullopt, or getSound returns nullptr error desciprtion can be optained with

```cpp
std::string getError() const;
```

- example:
```cpp
void playWithCache() {
  const Frequency sampleRate = Frequency::Hz(44100);
  AudioEngine engine(sampleRate);
  auto mixer = engine.addMixer(FrameFormat::Stereo);
  //engine.addMixerOutput // some output

  SoundCache cache;

  // flac has default loadLoops = false, that makes it incompatible with function definition that SoundCache wants
  cache.addLoadingFunction(".flac", 
    [](const std::filesystem::path& path) {
      return FlacDecoder::load(path);
    }
  ); 
  cache.addLoadingFunction(".wav", WavDecoder::load);
  cache.addLoadingFunction(".mp3", Mp3Decoder::load);

  auto sound1 = cache.add("sound.flac");
  auto sound2 = cache.add("sound.wav");
  auto music = cache.add("music.mp3", SoundCache::OpenMode::Stream);

  if(!sound1 || !sound2 || !music) {
    std::cout << cache.getError();
    return;
  }

  if(auto in = cache.getSound(*sound1)) {
    engine.play(mixer, engine.addInput(std::move(in)));
  }
  else {
    std::cout << cache.getError();
    return;
  }

  if(auto in = cache.getSound(*sound2)) {
    engine.play(mixer, engine.addInput(std::move(in)));
  }
  else {
    std::cout << cache.getError();
    return;
  }

  if(auto in = cache.getSound(*music, true, true)) {
    engine.play(mixer, engine.addInput(std::move(in)));
  }
  else {
    std::cout << cache.getError();
    return;
  }
}
```

---
### Recording:
To use microphone/instrument/something soundcard input some external library/system api needs to be used.
There are already AudioInput for SDL and AudioInput for PortAudio

- SDL input and output:

```cpp
SDL_IO {
public:
  bool init(Frequency sampleRate_p);                                   // inits SDL audio returns true on success, sampleRate must be the AudioEngine operating sampleRate, because SDL handles sample
                                                                       // rate conversion automatically, error can be checked wit getError

  std::unique_ptr<SDL_Output> createDefaultOutput(FrameFormat format); // creates default output device with frame format returns nullptr failure, error can be checked with getError
  std::unique_ptr<SDL_Input> createDefaultInput(FrameFormat format);   // creates default output device with frame format returns nullptr failure, error can be checked with getError
  std::string getError() const;
};
```

It is very simple to use just init with AudioEngine its used with sampleRate and call createDefaultOutput/Input or both destroying the SDL_IO will turn off its outputs and inputs, so it should
outlive them in most cases, both input and output are blocking
- problem: currently both input and output are blocking but they don't use callbacks but queuing mechanism which works great for output, but can have potentiall delay issues with input
- probably changing input to use callback could potentially resolve the issue, but there still exists problem with unkown buffer sizes of the outputs and inputs

- example:
```cpp

int main() {
  using namespace ZAudio;
  SDL_IO io;
  auto r = io.init();
  if(!r) {
    std::cout << r.getDescription();
    return 0;
  }
  int i = 0;
  for(auto& device : io.getInputDevices()) {
    std::cout << '(' << i << ')' << device.name << "\n";
  }
  int n;
  std::cin >> n;
  assert(n >= 0 && n < io.getInputDevices().size());
  auto input = io.addAudioInput(2, 512, n);
  if(!input) {
    std::cout << io.getError();
    return 0;
  }
  // engie.addInput(input) etc.
}

```

PortAudioIO lets you choose the audio device with deviceIndex, also audio devices can be set to be blocking or not

```cpp
class PortAudioIO {
public:
Result init(); // must be called before anything other
Result startInputOutput(int32_t numberOfInputChannels_p, int32_t numberOfOutputChannels_p, int32_t bufferSize);  // starts both input and output with default devices
Result startOnlyInput(int32_t numberOfChannels, int32_t bufferSize);                                             // starts only input with default device
Result startOnlyOutput(int32_t numberOfChannels, int32_t bufferSize);                                            // starts only output with default device

Result startInputOutput(int32_t inputDeviceIndex, int32_t numberOfInputChannels_p, int32_t outputDeviceIndex, int32_t numberOfOutputChannels_p, int32_t bufferSize);
// starts both input and output input with choosen devices

Result startOnlyInput(int32_t deviceIndex, int32_t numberOfChannels, int32_t bufferSize);
// starts only input input with choosen devices

Result startOnlyOutput(int32_t deviceIndex, int32_t numberOfChannels, int32_t bufferSize);
// starts only output input with choosen devices

void stop();
// stops current io, now startInputOutput/startInput/startOutput can be called again

const std::vector<HostApi>& getHostApis() const;
const std::vector<Device>& getDevices() const;
std::optional<int32_t> getDefaultInputDevice() const;
std::optional<int32_t> getDefaultOutputDevice() const;
std::unique_ptr<CallbackInput> getAudioInput(bool blocking = true);  // returns AudioInput, if init and startSomething were success it should never return nullptr
std::unique_ptr<CallbackOutput> getAudioOutput(bool blocking = true);// returns AudioOutpout, if init and startSomething were success it should never return nullptr
}
```

startInputOutput/startOnlyInput/startOnlyOutput can be called only once, and then stop needs to be called before another call.
PortAudioIO provides only single input and single output and getAudioInput.
Here is device and hostapi information:

```cpp
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
```

- example:
```cpp
int main() {
  using namespace ZAudio;
  PortAudioIO io;
  auto r = io.init();
  if(!r) {
    std::cout << r.getDescription();
    return 0;
  }
  int i = 0;
  for(auto& device : io.getDevices()) {
    std::cout << '(' << i << ')' << device.name << "\n";
  }
  int n;
  std::cin >> n;
  assert(n >= 0 && n < io.getDevices().size());

  r = io.startOnlyInput(n, 2, 512);
  if(!r) {
    std::cout << r.getDescription();
    return 0;
  }

  auto input = io.getAudioInput();
  // engie.addInput(input) etc.
}
```

## Audio Output
AudioOutput is interface, that represents any output (for example speakers, to file, to buffer etc.). It is important to note, that AudioEngine (or user if used without AudioEngine), needs to call setSampleRate
before using it. AudioEngine does it automatically, and if sample rate of output is diffrent than set, AudioOutput must convert it itself.
```cpp
class AudioOutput {
public:
  virtual ~AudioOutput() {}

  // send in frame
  virtual void send(std::span<const sample_t> in) = 0;

  virtual void setSampleRate(Frequency sampleRate) = 0;

  // set parameter(no need ot override if there aren't any parameters to set)
  virtual void setParameter(size_t id, ParameterValue value) {}

  // get some output value (no need ot override if there aren't any values to
  virtual ParameterValue getOutputValue(size_t id) { return ParameterValue(); }

  // return true if some error occured
  virtual bool errorOccured() const = 0;

  // return true if ended
  virtual bool ended() const = 0;

  // returns input format
  virtual FrameFormat getFormat() const = 0;
};
```
To make own output just dervie form AudioOutput.

### FileOutput

- file input actions:
```cpp
enum : uint32_t {
  StopID
};
```

- api of FileOutput
```cpp
FileOutput(std::unique_ptr<AudioEncoder> encoder_p);

// just AudioOutput
void send(std::span<const sample_t> in) override;
void setSampleRate(Frequency sampleRate) override;
void setParameter(size_t id, ParameterValue value) override;
ParameterValue getOutputValue(size_t id) override;
bool errorOccured() const override;
bool ended() const override;
FrameFormat getFormat() const override;
```

- Similiary to input there FileOutput uses AudioEncoder(instead of decoder) class.

```cpp
class AudioEncoder {
public:
  virtual ~AudioEncoder() = default;
  virtual void send(std::span<const sample_t> out) = 0;
  virtual Frequency getSampleRate() const = 0;
  virtual FrameFormat getFormat() const = 0;
  virtual bool errorOccured() const = 0;
  virtual bool ended() const = 0;
};
```

currently only format that supports encoding is wav, to make output from wav create WavEncoder

```cpp
static ResultValue<std::unique_ptr<AudioEncoder>> WavEncoder::create(Frequency sampleRate_p, FrameFormat format_p, const std::filesystem::path& path_p);
static ResultValue<std::unique_ptr<AudioEncoder>> createWithStream(Frequency sampleRate_p, FrameFormat format_p, std::unique_ptr<FileOutputStream> fileOutput);
```

There is possibility to use FileOutputStream interface for saving audio:

```cpp
class FileOutputStream {
public:
enum struct SeekOrigin {
  Start, Current
};
  virtual size_t write(const uint8_t* data, size_t n) = 0;
  virtual bool seek(size_t pos, SeekOrigin origin) = 0;
private:
};
```

To make wav output just:

```cpp
void record() {
  AudioEngine engine = ...
  MixerHandle mixer = ...
  // something is playing on mixer
  auto wav = WavEncoder::create(Frequency::Hz(44100), FrameFormat::Stereo, "recording.wav");
  if(!wav) {
    std::cout << wav.getDescription();
    return;
  }
  engine.play(mixer, engine.addOutput<FileOutput>(wav.get()));
}
```

For recording to buffer there is BufferEncoder:
```cpp
BufferEncoder(SoundBuffer& sound_p)

// example:

void record(AudioEngine& engine, MixerHandle& mixer, SoundBuffer& buffer) {
  engine.play(mixer, engine.addOutput<FileOutput>(std::make_unique<BufferEncoder>(buffer)));
}
```
It is also possible to use encoders to save sound from SoundBuffer with encodeSound method:

```cpp
Result encodeSound(AudioEncoder& encoder, const SoundBuffer& soundBuffer);
```

### Speaker Output

To output to speakers external libraries will be used, there is already support for SDL and PortAudio. How to uses these is already showed in AudioInput - recording.

- example:
```cpp
int main() {
  using namespace ZAudio;
  const Fequency sampleRate = Frequency::Hz(44100);
  AudioEngine engine(sampleRate);
  auto mixer = engine.addMixer(FrameFormat::Stereo);

  SDL_IO io;
  if(!io.init(sampleRate)) {
    std::cout << io.getError();
    return 0;
  }
  auto sdlout = io.createDefaultOutput(FrameFormat::Stereo);
  if(!sdlout) {
    std::cout << io.getError();
  }

  auto out = engine.addOutput(std::move(sdlout));

  engine.play(mixer, *someInput*);
  engine.addMixerOutput(mixer, out);
  std::this_thread::sleep_for(std::chrono::seconds(10));
}
```

## Effects

### Effect

Effect is interface that can be used to change sound. Engine class automatically calls setSampleRate, but for using them manually it needs to be called before use.

```cpp
class Effect {
public:
  virtual ~Effect() {}

  // returns output format of effect for example stereo or mono
  virtual FrameFormat getOutputFormat() const = 0;

  // returns output format of effect for example stereo or mono
  virtual FrameFormat getInputFormat() const = 0;

  // applies effect and fills out with result
  virtual void process(std::span<const sample_t> in, std::span<sample_t> out) = 0;

  // returns how long effect wil generate output after input stopped
  virtual uint32_t getTailTime() const = 0;

  // sets parameter with id
  virtual void setParameter(size_t id, ParameterValue value) = 0;

  // used for effects that have effects inside, for example SerialEffect, it is used to set effect of effect with some index, because of that for now only 1 level depth is supported
  virtual void setParameter(size_t id1, size_t id2, ParameterValue value) {}

  // returns some output value based on id
  virtual ParameterValue getOutputValue(size_t id) { return ParameterValue(); }

  // sets sample rate - must be called before processing (adio engine does this automatically)
  virtual void setSampleRate(Frequency sampleRate) = 0;

  // return deep copy of effect
  virtual std::unique_ptr<Effect> clone() const = 0;

  // saves effect to tree structure
  virtual Result save(Tools::TreeDatabaseWriter writer) const = 0;

  // loads effect to tree structure
  virtual Result load(Tools::TreeDatabaseReader reader) = 0;

  // returns id(name) of effect that is used recognize effect
  virtual std::string getID() const = 0;

  // returns id - it should change, whenever save/load becomes incompatible with previous
  virtual int64_t getVersion() const = 0;


  // saves effect to xml (outstream should be something like cout or ofstream)
  template<typename Out>
  static Result saveEffectToXML(const Effect& effect, Out& outStream);

  // loads effect from xml (inStream should be something like cin or ifstream)
  template<typename In>
  static ResultValue<std::unique_ptr<Effect>> loadEffectFromXML(In& inStream)
};
```

- If effect has stereo output, but is used as mono it wil automatically be mixed, so its better to do stereo if it can
- Similarly if input is stereo and used as mono it will automatically be splited


Usually effect has:
- nested class Parameters that is only parameter to contructor, and specifies whole effect
- unscoped enum or constexprs for parameters ids and getting values, for example:
```cpp
enum : uint32_t {
  Parameter1,
  Parameter2,
  GetSomeValue1,
  GetSomeValue2
};


// now to set effect parameter:
effect.setParameter(SomeEffect::Parameter1, SomeValue);

//or with audio engine
engine.setEffectParameter(effect, SomeEffect::Parameter1, SomeValue);
```

Documentation of effects:
- (Nearly) all effects have sound samples and parameters used in making them.
- EffectExamples/nameOfEffect

---


### AutoWahEffect
AutoWah uses AudioDetector to set frequency of band pass filter. So it changes frequency depending of volume of incoming signal.

- AutoWah::Parameters
```cpp
// range of envelope used for modulation (if envelope is outside it i capped), so good idea is to set frequency range to wanted and envelope range to range of instrument volume(so there is no problem with silence)
Volume lowEnvelope = Volume::dB(-27);
Volume highEnvelope = Volume::dB(-5);

// time after which cutoff frequency drops
Time decay = Time::miliseconds(200);

// range of cutoff frequency
Frequency minFrequency = Frequency::Hz(300);
Frequency maxFrequency = Frequency::Hz(4000);

// q coefficient of filter
double q = 2.;
Parameters();
Parameters(Volume lowEnvelope_p, Volume highEnvelope_p, Time decay_p, Frequency minFrequency_p, Frequency maxFrequency_p, double q_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  LowEnvelopeID,
  HighEnvelopeID,
  DecayID,
  MinFrequencyID,
  MaxFrequencyID,
  Q_ID
};
```

---

### BitCrusherEffect
BitCrusher reduces bit depth of audio.

- BitCrusherEffect::Parameters
```cpp
double bitDepth = 16.;          // bitDepth that audio is reduced to - can be not total
Volume wet = Volume::linear(1); // gain of processed sygnal
Volume dry = Volume::linear(0); // gain of unprocessed sygnal
```
- parameters IDs:
```cpp
enum : uint32_t {
  BitDepthID,
  WetID,
  DryID
};
```
---

### BypassEffect
Bypass effect doesn't do anything other than automatically forcing conversion of FrameFormats, to construct it just pass input and output formats
```cpp
BypassEffect(FrameFormat inputFormat_p, FrameFormat outputFormat_p);
```

---



### DelayEffect
Simple delay(echo) effect with feedback.
- DelayEffect::Parameters:
```cpp
Time delayTime;                             // duration of delay
Time reservedDelayTime = Time::seconds(2.); // reserved time for delay, similiar as in std::vector, it can prevent allocations. Matters only when delay is changed
Volume dry = Volume::linear(0.5);           // gain of unprocessed signal
Volume wet = Volume::linear(0.5);           // gain of processed signal

Volume feedback = Volume::dB(-3);           // gain of delayed signal going back to delay, gain of echos
// linearValue = 0      | dB ~ -96 => no echo
// 0 < linear value < 1 | dB < 0   => echo
// linear > 1           | dB > 0   => it will grow and become lauder and lauder (not needed for anything probably)

Parameters();
Parameters(Time delayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  DelayTimeID,
  ReservedDelayTimeID,
  DryID,
  WetID,
  FeedbackID
};
```

---

### DuckDelayEffect
Delay effect, but when input is loud enough it mutes itself

- Delay::Parameters:
```cpp

// duration of delay
Time delayTime;

// reserved time for delay, similiar as in std::vector, it can prevent allocations. Matters only when delay is changed
Time reservedDelayTime = Time::seconds(2);

// gain of unprocessed signal
Volume dry = Volume::dB(0);

// gain of delayed signal going back to delay, gain of echos
Volume feedback = Volume::dB(-3);

// range of processed(delay) signal gain
Volume wetMin = Volume::dB(-12);
Volume wetMax = Volume::dB(0);

// if louder wet = wetMin
Volume threshold = Volume::dB(-6);

// specify how long should it take to go from wetMin to wetMax
Time recoveryTime = Time::seconds(0.2);

// specify how fast audio detector reacts
Time attackTime = Time::miliseconds(50);
Time releaseTime = Time::miliseconds(50);

Parameters();
Parameters(Time delayTime_p);
Parameters(Time delayTime_p, Volume dry, Volume feedback);
Parameters(Time delayTime_p, Volume dry_p, Volume feedback_p, Volume wetMin_p, Volume wetMax_p, Volume threshold_p, Time recoveryTime_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  DelayTimeID,
  ReservedDelayTimeID,
  DryID,
  FeedbackID,
  WetMinID,
  WetMaxID,
  ThresholdID,
  RecoveryTimeID,
  AttackTimeID,
  ReleaseTimeID
};
```

---

### DynamicsProcessorEffect
DynamicsProcesorEffect(Compressor or Expander) is used to adjust gain based on envelope of input signal. In example compressor effect makes everything same volume, while exapdner with hard stop works as noise gate.


- DynamicsProcessorEffect::Type
```cpp
enum struct Type {
  Compressor,
  Expander
};
```

- DynamicsProcessorEffect::Parameters
```cpp
// gain applied after processing
Volume outputGain = Volume::dB(0);

// attack time of audio detector
Time attackTime = Time::miliseconds(50);

// release time of audio detector
Time releaseTime = Time::miliseconds(50);

// for compresssor when above signal is beeing attenuated
// for expander when below signal is beeing attenuated
Volume threshold = Volume::dB(-3);

// should be > 1., the larger the more compressor/expander attenuates
double ratio = 1.;

// if true works like ratio = infinite, compressor caps to threshold, and expander is noise gate
bool hardStop = false;

// type - compressor or expander
Type type;

Parameters();
Parameters(Volume outputGain_p, Time attackTime_p, Time releaseTime_p, Volume threshold_p, double ratio_p, bool hardStop_p, Type type_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  OutputGainID,
  AttackTimeID,
  ReleaseTimeID,
  ThresholdID,
  RatioID,
  HardStopID,
  TypeID
};
```

---
### FilterEffect
FilterEffect provides multiple analog style filters.

- FilterEffect::Parameters
For more detail see [AnalogFilter](#analogfilter).
```cpp
Type type = Type::ByPass;
Frequency frequency;
double q = 0.;
Volume gain;
Parameters();
Parameters(Type type_p, Frequency frequency_p, double q_p = 0., Volume gain_p = Volume::dB(0));
```
- parameters IDs:
```cpp
enum : uint32_t {
  TypeID,
  FrequencyID,
  Q_ID,
  GainID
};
```
---


### FlangerEffect
Uses modulated delay to create flanger effect

- FlangerEffect::Parameters
```cpp

Frequency rate = Frequency::Hz(0.4);   // rate of modulation
Time minDelay = Time::miliseconds(0.);
Time maxDelay = Time::miliseconds(7.);
Volume feedback = Volume::dB(-3);

Parameters();
Parameters(Frequency rate_p, Time minDelay_p, Time maxDelay_p);
Parameters(Frequency rate_p, Time minDelay_p, Time maxDelay_p, Volume feedback_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  RateID,
  MinDelayID,
  MaxDelayID,
  FeedbackID
};
```
---

### LfoWahEffect
LfoWahEffect modulates band pass filter  with lfo

- LfoWahEffect::Parameters
```cpp
Frequency rate = Frequency::Hz(2);            // rate of modulation
Frequency minFrequency = Frequency::Hz(350);  // min fc of filter
Frequency maxFrequency = Frequency::Hz(2000); // max fc of filter

Parameters();
Parameters(Frequency rate_p, Frequency minFrequency_p, Frequency maxFrequency_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  RateID,
  MinFrequencyID,
  MaxFrequencyID
};
```

---

### LooperEffect
LooperEffect works just like normal looper, it lets:
- record sound
- loop it
- record on top of it
---
- LooperEffect::Mode
```cpp
enum struct Mode {
  Paused,
  Recording,
  Overdubbing,
  Playing
};
```

- LooperEffect::Parameters
```cpp
Time maxDuration = Time::seconds(1);
Volume dry = Volume::dB(0);
Volume wet = Volume::dB(0);

Parameters();
Parameters(Time maxDuration_p, Volume dry_p, Volume wet_p);
```

- parameters IDs:
```cpp
MaxDurationID,
DryID,
WetID,
ModeID,
ClearID  // to clear call setParameter(ClearID, true)
```
---

Possible LooperEffect states:
1. Paused - just passes input to output, no recording or playback
2. Recording - to set call setParameter(LooperEffect::ModeID, LooperEffect::Mode::Playing) - it erases all recorded previously and starts recording,
ends when max time is achieved or mode is changed
1. Overdubbing - records on top of previously recorded sound
2. Playing - no recording but playing what was recorded by far

### MonoToStereoAdapter
This effect takes single mono to mono effect and makes same effect, but stereo effect from two of these. All setParameter etc. are forwarded same to left and right effects.

- to construct just use constructor:
```cpp
MonoToStereoAdapter(const Effect& effect)
```
- every method that works for mono effect, should work for MonoToStereoAdapter, just forward to left and right, for example:
```cpp

  VolumeControl effect(Volume::db(3));

  MonoToStereoAdapter adapter(effect); // adapter clones effect, doesn't hold reference

  adapter.setParameter(VolumeControl::VolumeChangeID, Volume::dB(-3)); // now both right and left channel have VolumeChange = -3dB
```

---

### ParallelEffect
Parallel effect is used to make one effect from many effects. It splits signal to every effect and then mixes it down.
```
                        -> effect1 ->
Parallel Effect : input -> effect2 -> * (1/3) -> out
                        -> effect3 ->
```

- It behaves just like normal effects, can be saved(all effects that are inside must be added to EffectSerializer).

- to construct pass inputFormat, outputFormat and number of effects:
```cpp
ParallelEffect(FrameFormat inputFormat_p, FrameFormat outputFormat_p, size_t n)
```
---
- to set effect use setEffect method, where i is effect index and effect is new effect
```cpp
setEffect(size_t i, std::unique_ptr<Effect> effect)
```
---
- to set nested effect parameter use setParameter method, where effectIndex is index of nested effect
```cpp
setParameter(size_t effectIndex, size_t parameterID, ParameterValue value)
```

- it also automatically sets sample rates of nested effects
---
- example:
```cpp

void pseudoReverb(std::vector<sample_t>& buffer, Frequency sampleRate) {
  using namespace ZAudio;
  const size_t numberOfEffects = 4;
  ParallelEffect parallel(FrameFormat::Mono, FrameFormat::Mono, numberOfEffects);
  parallel.setSampleRate(sampleRate);

  DelayEffect::Parameters parameters;

  parameters.delayTime = Time::seconds(0.2);
  parallel.setEffect(0, std::make_unique<DelayEffect>(parameters));

  parameters.delayTime = Time::seconds(0.3);
  parallel.setEffect(1,std::make_unique<DelayEffect>(parameters));

  parameters.delayTime = Time::seconds(0.5);
  parallel.setEffect(2,std::make_unique<DelayEffect>(parameters));

  parameters.delayTime = Time::seconds(0.7);
  parallel.setEffect(3,std::make_unique<DelayEffect>(parameters));

  // to change parameter
  parallel.setParameter(3, DelayEffect::DelayTimeID, ParameterValue::time(Time::seconds(1.)));

  for(auto& v : buffer) {
    std::array<sample_t, 1> in = {v};
    std::array<sample_t, 1> out;
    parallel.process(in, out);
    v = out.front();
  }
}

```

---


### PhaserEffect
Phaser effects uses modulated all pass filters to create "sweeping phasing effect".

- PhaserEffect::Parameter:
```cpp
struct Parameters {
  Frequency rate = Frequency::Hz(0.4); // how fast filters change fc
  double depth = 1.;                   // how wide modulation is
  Volume wet = Volume::dB(-3);         // gain of processed signal
  Volume dry = Volume::dB(-3);         // gain of unprocessed signal
  Parameters();
};
```

- parameters Ids:
```cpp
enum : uint32_t {
  RateID,
  DepthID,
  WetID,
  DryID
};
```

---

### PitchShiftEffect
PitchShiftEffect lets you change pitch without changing duration of the sound. it uses PhaseVocoder to stretch/shrink signal and then resamples it.

PitchShiftEffect::Parameters
```cpp
double pitchShiftRatio = 1.;   // 2 is octave higher 0.5 is octave lower etc.
Type algorithm = Type::Normal; // algorithm used in correction of strecth/shrink in PhaseVocoder, see more in PhaseVocoder

Parameters();
Parameters(double pitchShiftRatio_p, Type algorithm_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  PitchShiftRatioID,
  AlgorithmID
};
```

### PingPongDelayEffect
Stereo delay with diffrent delays on each channel, creating spatial effect

- PingPongDelayEffect::Parameters
```cpp
Time leftDelayTime;                        // delay time of left channel
Time rightDelayTime;                       // delay time of right channel
Time reservedDelayTime = Time::seconds(4); // reserved time for delay, similiar as in std::vector, it can prevent allocations. Matters only when delay is changed
Volume dry;                                // gain of processed signal
Volume wet;                                // gain of unprocessed signal
Volume feedback;                           // gain of delayed signal going back to delay, gain of echos

Parameters();
Parameters(Time leftDelayTime_p, Time rightDelayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p);
Parameters(Time leftDelayTime_p, Time rightDelayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p, Time reservedDelayTime_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  LeftDelayTimeID,
  RightDelayTimeID,
  ReservedDelayTimeID,
  DryID,
  WetID,
  FeedbackID
};
```
---

### ReverseDelayEffect
ReverseDelayEffect plays reversed sound with some delay.

- ReverseDelayEffect::Parameters
```cpp
Time delayTime;
Volume dry;
Volume wet;
Volume feedback;

Parameters()l
Parameters(Time delayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p);
```

- parameters IDs:
```cpp
DelayTimeID,
DryID,
WetID,
FeedbackID
```

---

### RobotEffect
Effect that creats "robot" sound with phase vocoder, by resetting phases to 0.

- RobotEffect::Parameters
```cpp
size_t frameSizeTwoPow = 11; // frame size is 2^frameSizeTwoPow so 2^11=2048
double hopRatio = 1.;        //offset between fft frames in phase vocoder, clamped to (0, 1> range
Parameters();
explicit Parameters(size_t frameSize_p, double hopRatio_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  FrameSizeTwoPowID,
  HopRatioID
};
```

---



### SequenceFilterEffect
Effect that randomaly changes band filter frequency


- SequenceFilterEffect::Parameters:
```cpp
Frequency changeFrequency = Frequency::Hz(7); // how often frequency changes

                                              // range of frequency
Frequency minFrequency = Frequency::Hz(100);
Frequency maxFrequency = Frequency::Hz(1000);

double filterQ = 5.;                          // filter q parameter

Parameters();
Parameters(Frequency changeFrequency_p, Frequency minFrequency_p, Frequency maxFrequency_p, double filterQ_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  ChangeFrequencyID,
  MinFrequencyID,
  MaxFrequencyID,
  FilterQ_ID
};
```

---

### SerialEffect
SerialEffect is used to make one effect from many effects. It processes signal with each effect sequentially.
```
SerialEffect : input -> effect1 -> effect2 -> -> effect3 -> out
```

- It behaves just like normal effects, can be saved(all effects that are inside must be added to EffectSerializer).

- to construct pass number of effects:
```cpp
SerialEffect(size_t n);
```
---
- to set effect use setEffect method, where i is effect index and effect is new effect
```cpp
setEffect(size_t i, std::unique_ptr<Effect> effect)
```
---
- to set nested effect parameter use setParameter method, where effectIndex is index of nested effect
```cpp
setParameter(size_t effectIndex, size_t parameterID, ParameterValue value)
```

--- 
- it is also possible to turn off some effect (set it to bypass)
```cpp
enum : uint32_t {
  StartBypassingEffect,
  StopBypassingEffect
};
setParameter(StartBypassingEffect, ParameterValue::integer(0)) // starts bypassing first effect
```

- it also automatically sets sample rates of nested effects
---
- example:
```cpp

void echoRobot(std::vector<sample_t>& buffer, Frequency sampleRate) {
  using namespace ZAudio;
  const size_t numberOfEffects = 2;
  SerialEffect serial(numberOfEffects);
  serial.setSampleRate(sampleRate);

  RobotEffect::Parameters p1;
  p1.hopRatio = 0.5;

  serial.setEffect(0, std::make_unique<RobotEffect>(p1));

  DelayEffect::Parameters p2;
  p2.delayTime = Time::seconds(1.);

  serial.setEffect(1, std::make_unique<DelayEffect>(p2));

  for(auto& v : buffer) {
    std::array<sample_t, 1> in = {v};
    std::array<sample_t, 1> out;
    serial.process(in, out);
    v = out.front();
  }
}

```


---


### Spatial2dEffect
Spatial2dEffect provides some sound location, but only in 2d space (just half of circle, second half is the same). It uses delay between ears and some gain and frequency change.
- Spatial2dEffect::Parameters
```cpp
Time maxEarDelay = Time::miliseconds(0.6);       // so it should be ~ time that sound takes to go from one ear to another, 0.6 is ok value
Volume minEarGain = Volume::dB(-1.);             // how much gain changes depending of position of sound, 0 will work but some small value can help
Volume maxFilteredPart = Volume::linear(0.3);    // how much low pass is used depending of position of sound, 0 will work but some small value like can help
Frequency lowPassFrequency = Frequency::Hz(300); // frequency of lowpass
double soundAngle = 0.;                          // where the sound is coming from in radians (0. is front -PI/2 is left PI/2 is right)
```

- Parameters IDs:
```cpp
enum : uint32_t {
  MaxEarDelayID,
  MinEarGainID,
  MaxFilteredPartID,
  LowPassFrequencyID,
  SoundAngleID
};
```

---



### StereoChorusEffect:
Chorus effect, similiar to flanger but tries to simulate multiple detunet sources effect.

- StereoChorusEffect::Parameters:
```cpp
Frequency rate = Frequency::Hz(2);     // rate of modulation
Time minDelay = Time::miliseconds(3.);
Time maxDelay = Time::miliseconds(7.);
Parameters();
Parameters(Frequency rate_p, Time minDelay_p, Time maxDelay_p);
```

---

### StereoFlangerEffect:
Stereo version of flanger effect - uses two flangers with phase offset. Other than that usage is same [FlangerEffect](#flangereffect).

---

### StereoPhaserEffect:
Stereo version of phaser effect - uses two phasers with phase offset. Other than that usage is same [PhaserEffect](#phasereffect).

---

### TremoloEffect
Tremolo effect modulates gain of sound.

- TremoloEffect::Parameters:
```cpp
// how fast gain changes
Frequency frequency = Frequency::Hz(1);

// range of gain value
Volume minGain;
Volume maxGain;

Parameters();
Parameters(Frequency frequency_p, Volume minGain_p, Volume maxGain_p);
```

- Parameters IDs:
```cpp
enum : uint32_t {
  FrequencyID,
  MinGainID,
  MaxGainID
};
```

---


### TubePreampEffect
TubePreampEffect is used to simulate tube distortion.

- TubePreampEffect::Parameters:
```cpp
Volume inputGain;
Volume outputGain;

// frequency range of input (so it should work well for bass aswell as guitar etc.), for guitar it is 80Hz - 1200Hz
Frequency frequencyRangeMin;
Frequency frequencyRangeMax;
Volume low;                   // how much low frequencies
Volume high;                  // how much high frequencies
```

- Parameters IDs:
```cpp
enum : uint32_t {
  InputGainID,
  OutputGainID,
  FrequencyRangeMinID,
  FrequencyRangeMaxID,
  LowID,
  HighID
};
```

### VolumeControlEffect
VolumeControlEffect is used to apply some gain change on sound. The volumeChange value is current volumeChange (for example 0dB means that this effect doesn't havey any effect :)), 
The maxChangePerSecond value describes how fast volume is changing after setting it, for example it can be used to fade-out instead of instant mute of sound.
It is also possible to set volume change instantly with SetVolumeChangeNoSmoothingID.

- VolumeControlEffect::Parameters
```cpp
Volume volumeChange;
Parameters();
Parameters(Volume volumeChange_p, Volume maxChangePerSecond_p = Volume::dB(30));
```

- Parameters IDs:
```cpp
VolumeChangeID,
MaxChangePerSecondID,
SetVolumeChangeNoSmoothingID
```

### WhisperEffect
Effect that "whispering" sound with phase vocoder, by setting random phases.

- WhisperEffect::Parameters
```cpp
size_t frameSizeTwoPow = 11; // frame size is 2^frameSizeTwoPow so 2^11=2048
double hopRatio = 1.;    //offset between fft frames in phase vocoder, clamped to (0, 1> range
Parameters();
explicit Parameters(size_t frameSize_p, double hopRatio_p);
```

- parameters IDs:
```cpp
enum : uint32_t {
  FrameSizeTwoPowID,
  HopRatioID
};
```

---


## Tools

Tools are classes and functions, that can be used in creating effects etc.

---

### AnalogFilter
Biquad filter that simulates various analog filters.
- AnalogFilter:
```cpp
//public methods:
AnalogFilter();                                             // constructs bypass filter
explicit AnalogFilter(AnalogFilter::Parameters parameters); // constructs from parameters
void reset(AnalogFilter::Parameters parameters);            // resets parameters
sample_t process(sample_t in);                              // processes sample and returns result
```
- AnalogFilter::Type:
```cpp
 enum struct Type {
    ByPass,   // doesn't do anything
    AllPass,  // no frequency response change, but phase response
    FirstOrderLowPass,  // attenuates frequencies higher than fc
    FirstOrderHighPass, // attenuates frequencies lower than fc
    LowPass,  // attenuates frequencies higher than fc (also has q control, that controls peek at fc)
    HighPass, // attenuates frequencies lower than fc (also has q control, that controls peek at fc)
    LowShelf, // boosts or attenuates frequencies below fc
    HighShelf,// boosts or attenuates frequencies above fc
    BandPass, // attenuates frequencies outside range, q controls how big and steep the range is
    BandStop  // attenuates frequencies inside range, q controls how big and steep the range is
  };
```
- AnalogFilter::Parameters:
```cpp
// public fields:
AnalogFilter::Type;   // type of filter AllPass, LowPass etc.
Frequency sampleRate; // sample rate used by filter
Frequency frequency;  // cutoff frequency of filter used in all filters
double q = 0.707;            // q value of filter, used in band pass, and band stop, controls how steep band is
Volume gain;    // gain used in high shelf and low shelf, controls gain

// public methods:
Parameters(Type type_p, Frequency sampleRate_p, Frequency frequency_p, double q_p = 0., Volume gain_p = Volume::dB(0));
static Parameters createByPassParameters();
static Parameters createAllPassParameters(Frequency sampleRate, Frequency frequency);
static Parameters createLowPassParameters(Frequency sampleRate, Frequency frequency, double q = 0.707);
static Parameters createHighPassfParameters(Frequency sampleRate, Frequency frequency, double q = 0.707);
static Parameters createFirstOrderLowPassParameters(Frequency sampleRate, Frequency frequency);
static Parameters createFirstOrderHighPassfParameters(Frequency sampleRate, Frequency frequency);
static Parameters createLowShelfParameters(Frequency sampleRate, Frequency frequency, Volume gain);
static Parameters createHighShelfParameters(Frequency sampleRate, Frequency frequency, Volume gain);
static Parameters createBandPassParameters(Frequency sampleRate, Frequency frequency, double q);
static Parameters createBandStopParameters(Frequency sampleRate, Frequency frequency, double q);
```

- Example:

```cpp
// function that processes buffer with low pass filter
void processLowPass(Frequency sampleRate, Frequency cutOffFrequency, std::span<sample_t> buffer) {
  AnalogFilter filter(AnalogFilter::Parameters::createLowPassParameters(sampleRate, cutOffFrequency)); // same as AnalogFilter filter(Parameters(AnalogFilter::Type::LowPass, sampleRate, frequency, 0.707))
  for(auto& v : buffer) {
    v = filter.process(v);
  }
}

// function that processes buffer with low shelf filter and then allpass
void processLowShelfAndAllpass(Frequency sampleRate, Frequency cutOffFrequency, Volume gain, std::span<sample_t> buffer) {
  AnalogFilter filter(AnalogFilter::Parameters::createLowShelfParameters(sampleRate, cutOffFrequency, gain)); // same as AnalogFilter filter(Parameters(AnalogFilter::Type::LowPass, sampleRate, frequency, 0., gain))
  for(auto& v : buffer) {
    v = filter.process(v);
  }
  filter.reset(AnalogFilter::Parameters::createAllPassParameters(sampleRate, cutOffFrequency));
  for(auto& v : buffer) {
    v = filter.process(v);
  }
}
```

---

### AudioDelay
AudioDelay handles delay(echo) with feedback, dry and wet parameters.
```cpp
// Frequency sampleRate   - sample rate of AudioDelay
// Time delayTime         - duration of delay
// Time reservedDelayTime - reserved time for delay, similiar as in std::vector, it can prevent allocations. Matters only when delay is changed
// Volume dry             - gain of unprocessed signal
// Volume wet             - gain of processed signal
// Volume feedback        - gain of delayed signal going back to delay, gain of echos
// linearValue = 0      | dB ~ -96 => no echo
// 0 < linear value < 1 | dB < 0   => echo
// linear > 1           | dB > 0   => it will grow and become lauder and lauder
AudioDelay(Frequency sampleRate_p, Time delayTime_p, Time reservedDelayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p);
void setDry(Volume dry_p);
void setWet(Volume wet_p);
void setFeedback(Volume feedback_p);
void setDelayTime(Time delayTime);

sample_t process(sample_t in);                                  // processes sample and returns result
double getDelayTimeInSamples() const;                           // returns delay in number of samples
double getRT60TimeInSamples() const;                            // returns RT60 time in number of samples
static Time calculateRT60Time(Time delayTime, Volume feedback); // calculates RT60 time from delayTime and feedback
```
- example:
``` cpp
// returns buffer with added echos
std::vector<sample_t> addLongEcho(std::span<const sample_t> buffer, Frequency sampleRate) {
  // make delay with given sample rate, 1 second delay and quite big feedback
  AudioDelay delay(sampleRate, Time::seconds(1.), Time::seconds(1.), Volume::dB(-3), Volume::dB(-3), Volume::dB(-2));

  // size of buffer + number of samples of echos
  std::vector<sample_t> ans(buffer.size() + delay.getRT60TimeInSamples());
  for(size_t i = 0;  i < ans.size(); i++) {
    if(i < buffer.size()) {
      ans[i] = delay.process(buffer[i]);
    }
    else {
      // already used buffer, but neeed to include echos
      ans[i] = delay.process(0.);
    }
  }
  return ans;
}
```

### AudioDetector
AudioDetector (envelope detector) is used to detect envelope of input signal.

- AudioDetector::DetectMode
```cpp
enum struct DetectMode {
  Peak,          // envelope of signal
  MeanSquare,    // envelope of squared signal
  RootMeanSquare // root of envelope of squared signal
};
```
- AudioDetector
```cpp
// Frequency sampleRate  - sample rate of audio detecotr
// DetectMode detectMode - detect mode used in calculating enevelope
// Time attackTime       - how fast should envelope raise
// Time releaseTime      - how fast should envelope fall
// bool clamp            - should envelope be clamped to linear <0, 1>, dB < 0
AudioDetector(Frequency sampleRate_p, DetectMode detectMode_p, Time attackTime_p, Time releaseTime_p, bool clamp_p);
void setDetectMode(DetectMode detectMode_p);
void setAttackTime(Time attackTime_p);
void setReleaseTime(Time releaseTime_p);
void setClamp(bool clamp_p);
Volume process(sample_t in); // processes sample and returns calculated envelope
```
- example
```cpp

std::vector<Volume> getEnvelope(Frequency sampleRate, std::span<const sample_t> buffer) {
  AudioDetector detector(sampleRate, AudioDetector::DetectMode::RootMeanSquare, Time::miliseconds(50), Time::miliseconds(50), false);
  std::vector<Volume> ans(buffer.size());
  for(size_t i = 0; i < buffer.size(); i++) {
    ans[i] = detector.process(buffer[i]);
  }
  return ans;
}

void printChirpEnvelope() {
  // small buffer and sample rate, so example is readable
  int n = 32;
  Frequency sampleRate = Frequency::Hz(64);
  std::vector<sample_t> buffer(n);

  // fill with sine chirp
  std::cout << "CHIRP:\n";
  double f = 0.;
  const double step = (2. * M_PI) / 4;
  double volume = 0.;
  for(size_t i = 0; i < n; i++) {
    buffer[i] = sin(f) * volume;
    volume += 0.1;
    f += step;
  }

  // print result
  auto envelope = getEnvelope(sampleRate, buffer);
  std::cout.precision(3);
  for(auto& v : envelope) {
    std::cout << std::fixed << v.dB() << "dB " << v.linear() << " linear\n";
  }
}

void printToneEnvelope() {
  // small buffer and sample rate, so example is readable
  int n = 32;
  Frequency sampleRate = Frequency::Hz(64);
  std::vector<sample_t> buffer(n);

  // fill with just sine
  std::cout << "Tone:\n";
  double f = 0.;
  const double step = (2. * M_PI) / 4;
  double volume = 0.5;
  for(size_t i = 0; i < n; i++) {
    buffer[i] = sin(f) * volume;
    f += step;
  }

  // print result
  auto envelope = getEnvelope(sampleRate, buffer);
  std::cout.precision(3);
  for(auto& v : envelope) {
    std::cout << std::fixed << v.dB() << "dB " << v.linear() << " linear\n";
  }
}
```
- Output of printChirpEnvelope:
```
CHIRP:
-96.000dB 0.000 linear
-12.857dB 0.052 linear
-13.535dB 0.044 linear
-7.960dB 0.160 linear
-8.638dB 0.137 linear
-5.464dB 0.284 linear
-6.142dB 0.243 linear
-3.788dB 0.418 linear
-4.467dB 0.358 linear
-2.537dB 0.558 linear
-3.215dB 0.477 linear
-1.544dB 0.701 linear
-2.223dB 0.599 linear
-0.724dB 0.846 linear
-1.403dB 0.724 linear
-0.028dB 0.994 linear
-0.706dB 0.850 linear
0.576dB 1.142 linear
-0.102dB 0.977 linear
1.109dB 1.291 linear
0.431dB 1.104 linear
1.586dB 1.441 linear
0.907dB 1.232 linear
2.016dB 1.591 linear
1.338dB 1.361 linear
2.408dB 1.741 linear
1.730dB 1.489 linear
2.769dB 1.892 linear
2.090dB 1.618 linear
3.102dB 2.043 linear
2.424dB 1.747 linear
3.412dB 2.194 linear
```
- Output of printToneEnvelope
```
Tone:
-96.000dB 0.000 linear
-5.867dB 0.259 linear
-6.545dB 0.222 linear
-4.936dB 0.321 linear
-5.614dB 0.275 linear
-4.564dB 0.350 linear
-5.243dB 0.299 linear
-4.389dB 0.364 linear
-5.067dB 0.311 linear
-4.300dB 0.372 linear
-4.979dB 0.318 linear
-4.254dB 0.375 linear
-4.933dB 0.321 linear
-4.230dB 0.378 linear
-4.909dB 0.323 linear
-4.217dB 0.379 linear
-4.896dB 0.324 linear
-4.211dB 0.379 linear
-4.889dB 0.324 linear
-4.207dB 0.380 linear
-4.885dB 0.325 linear
-4.205dB 0.380 linear
-4.883dB 0.325 linear
-4.204dB 0.380 linear
-4.882dB 0.325 linear
-4.203dB 0.380 linear
-4.882dB 0.325 linear
-4.203dB 0.380 linear
-4.881dB 0.325 linear
-4.203dB 0.380 linear
-4.881dB 0.325 linear
-4.203dB 0.380 linear
```

---

### CallbackIO

These few classes are usefull for wrapping some external library (it is used for PortAudioIO).

- Class CallbackData
```cpp
// CallbackData public api:
void init(uint32_t numberOfChannels_p, uint32_t bufferSize);

template<typename T>
void inputCallback(std::span<const T> in);

template<typename T>
void outputCallback(std::span<T> in);

size_t getNumberOfChannels() const;

void setEnded();
```

- Class CallbackInput:
```cpp
CallbackInput(FrameFormat format_p, Frequency inSampleRate_p, std::shared_ptr<CallbackData> callbackData_p, bool blocking_p)
```
- Class CallbackOutput
```cpp
CallbackOutput(FrameFormat format_p, Frequency outSampleRate_p, std::shared_ptr<CallbackData> callbackData_p, bool blocking_p)
```

To make some input/output from external library that provides callback:

Make class Like SomeLibraryIO, it should containg shared_ptrs to CallbackData (one for input, one for output) and some wrapper around library.
Then to create AudioInput construct callback input from CallbackData.
Inside library callbacks just call CallbacData::inputCallback or CallbacData::outputCallback

- example:
```cpp

class SomeIO {
public:

  bool init() {
    // calls to initing library
  }

  const std::vector<Device>& getDevices();

  std::unique_ptr<AudioInput> addInput(/*some parameters*/, bool blocking) {
    // create input with library
    return std::make_unique<CallbackInput>(format, sampleRate, inData, blocking);
  }

  std::unique_ptr<AudioOutput> addOutput(/*some parameters*/, bool blocking) {
    // create output with library
    return std::make_unique<CallbackOutput>(format, sampleRate, outData, blocking);
  }

  ~SomeIO() {
    // close and wait till callbacks end (so there is no access to destructed CallbackData)
  }

private:
  std::shared_ptr<CallbackData> inData;
  std::shared_ptr<CallbackData> outData;
};

static void libraryInputCallback(void* userData, void* deviceBuffer, int length) {
  float* in = (float*)deviceBuffer;
  Tools::CallbackData* data = (Tools::CallbackData*) userData;

  data->inputCallback(std::span<const float>(in, in + (length / 4)));
}

static void libraryOutputCallback(void *userData, void* deviceBuffer, int length) {
  float* out = (float*)deviceBuffer;
  Tools::CallbackData* data = (Tools::CallbackData*) userData;

  data->outputCallback(std::span<float>(out, out + (length / 4)));
}

```

---

### CircularBuffer
Circular buffer(ring buffer) is basically queue, that works on array or a vector that lets you push infinitely and allows to get access of only n last elements.

- CircularBuffer<T> public api:
```cpp
void reset(size_t newSize); // clears buffer and resizes to new size

void resize(size_t newSize); // changes size of buffer, (if smaller leaving only newest elements)

void push(T v);

T get(size_t i); // returns nth value from newest (0 is last pushed)

T getFrictional(double offset) // performs linear interpolation in buffer, T must be convertable to double

Iterator current(); // returns iterator that points to last pushed element (iterator when increment goes towards older values)

size_t size() const; // returns size of buffer
```
```cpp
void circularBufferExample() {
  using namespace ZAudio::Tools;
  CircularBuffer<int> buffer(5);
  buffer.push(1);
  buffer.push(2);
  buffer.push(3);
  buffer.push(4);
  buffer.push(5);

  std::cout << buffer.get(0) << " ";
  std::cout << buffer.get(1) << " ";
  std::cout << buffer.get(2) << " ";
  std::cout << buffer.get(3) << " ";
  std::cout << buffer.get(4) << " ";
  std::cout << "\n";

  buffer.push(6);
  std::cout << buffer.get(0) << " ";
  std::cout << buffer.get(1) << " ";
  std::cout << buffer.get(2) << " ";
  std::cout << buffer.get(3) << " ";
  std::cout << buffer.get(4) << " ";
  std::cout << "\n";

  buffer.resize(2);
  std::cout << buffer.get(0) << " ";
  std::cout << buffer.get(1) << " ";
  std::cout << "\n";

  buffer.reset(4);
  std::cout << buffer.get(0) << " ";
  std::cout << buffer.get(1) << " ";
  std::cout << buffer.get(2) << " ";
  std::cout << buffer.get(3) << " ";
  std::cout << "\n";

  buffer.push(5);
  buffer.push(15);
  std::cout << buffer.getFrictional(0.5);
}
```

- output:
```
5 4 3 2 1
6 5 4 3 2
6 5
0 0 0 0
10
```

---

### DelayGainController
Used in DuckDelayEffect to control volume of delay

---

### EffectSerializer
EffectSerializer is singleton used for serializing and deserializing effects.
\
Normally to save/load effect just use [Effect](#effect) methods (saveEffectToXML, loadEffectFromXML). EffectSerializer is usefull when saving effect composed of other effects or not saving directly to file.

- it automatically adds all library effects
- external effects need to be added manually:
```cpp
// add effect type -> effectType can be any effect, it is used later for loading without switch statments
void EffectSerializer::addEffectType(const Effect& effectType);
```

- to get instance use:
```cpp
static EffectSerializer::EffectSerializer& instance();
```

- to load or save from TreeDatabaseWriter/TreeDatabaseReader
```cpp
Result save(Tools::TreeDatabaseWriter writer, const Effect& effect);
ResultValue<std::unique_ptr<Effect>> load(TreeDatabaseReader reader);
```

- to load or save from whole database - (loading assumes that saving was also done this way)
```cpp
Result save(Tools::TreeDatabase& database, const Effect& effect);
ResultValue<std::unique_ptr<Effect>> load(TreeDatabase& database);
```

---

### FFT
FFT provides fastFourierTransform and inverseFastFourierTransform methods,

ZAUDIO_USE_FFT define is needed to use this class (as well as effects that use this)

```cpp
// constructs fft object for use with given frame size, frame size should be power of two, for best performance
explicit FFT(size_t frameSize_p);

// move only
FFT(const FFT& oth) = delete;
FFT(FFT&& oth) noexcept;
FFT& operator= (const FFT& oth) = delete;
FFT& operator= (FFT&& oth) noexcept;

// does fast fourier transform on samples
void doFFT();

// does inverse fourier transform on bins
void doInverseFFT();

// returns reference to samples, that are used for fft
std::vector<sample_t>& getSamples();
const std::vector<sample_t>& getSamples() const;

// returns reference to bins - result of fft, and source to inverse fft
std::vector<std::complex<double>>& getBins();
const std::vector<std::complex<double>>& getBins() const;
```

- To use fft, first set values of samples, by modifying values of getSamples(),
then run doFFT(), get result with getBins()
- To use inverse fft, first set values of bins, by modifying values of getBins(),
then run doInverseFFT(), get result with getSamples()
- example:
```cpp
void fftExample() {
  const size_t frameSize = 1024;
  FFT fft(frameSize);

  // generate wave with nyquist frequency, it should be in frameSize / 2 bin
  double nv = -1;
  for(auto& sample : fft.getSamples()) {
    sample = nv;
    nv = -nv;
  }
  fft.doFFT();
  // we have result in fft.bins()

  std::cout << "Magniutde of nyquist: " << std::abs(fft.getBins()[frameSize / 2]) << "\n";
  std::cout << "Magniutde of dc offset: " << std::abs(fft.getBins()[0]) << "\n";
}
```
- Output of fftExample:
```
Magniutde of nyquist: 1
Magniutde of dc offset: 0
```
### FIR_Filter
FIR_Filter is just a fir filter with any number of coefficients. It just multiplies every coefficient(no fft etc.), so every process is O(n) where n is length of filter. It provides creation method to create sinc filter that is used for sample rate conversion.

- api:
```cpp
FIR_Filter();

// create FIR_Filter with given coeeficients
explicit FIR_Filter(const std::vector<double>& coefficients_p);

// create FIR_Filter as sinc low pass, with WindowFunction to choose and size
static FIR_Filter sincFilter(Frequency sampleRate, Frequency cutOffFrequency, uint32_t windowSize, WindowFunction::Type windowFunctionType);

// processes sample and returns result
sample_t process(sample_t in);
```

- example:
```cpp
void firFilterExample(std::vector<sample_t>& buffer, Frequency sampleRate) {
  // leave only frequencies < 100 Hz
  using namespace ZAudio;

  Tools::FIR_Filter filter = Tools::FIR_Filter::sincFilter(sampleRate, Frequency::Hz(100), 101, Tools::WindowFunction::Type::Blackman);
  for(auto& v : buffer) {
    v = filter.process(v);
  }
}
```

---

### PhaseShifter
Provides phaser effect for single channel, used in Phaser and StereoPhaser effects

- api:
```cpp
PhaseShifter();

// sampleRate_p  - sampleRate of phase shifter;
// depth_p       - how wide modulation is
// rate_p        - rate of modulation
// phaseOffset_p - starting phase of modulation
PhaseShifter(Frequency sampleRate_p, double depth_p, Frequency rate_p, double phaseOffset_p);

void setDepth(double depth_p);
void setRate(Frequency rate_p);

sample_t process(sample_t in);
```

- example:
```cpp
void phaseShifterExample(std::vector<sample_t>& buffer, Frequency sampleRate) {
  // apply phase effect
  using namespace ZAudio;

  Tools::PhaseShifter ps = Tools::PhaseShifter(sampleRate, 1., Frequency::Hz(1.), 0.);
  for(auto& v : buffer) {
    v = ps.process(v);
  }
}
```

---

### ReaderWriterQueue
ReaderWritereQueue is safe tread queue, only for one writer and one reader. It has limited size,

- ReadearWriterQueue<T> holds elements of type T

- construct with maximum number of elements
```cpp
ReaderWriterQueue(size_t size);
```
- to push to queue
```cpp
// (typename T2 for universal ref) tries to push returns true if pushed, false else (no place for push)
template<typename T2>
bool tryPush(T2&& v);

// waits for place to push and pushes
template<typename T2>
void waitAndPush(T2&& v)
```

- to pop from queue
```cpp
// tries to pop, if there is no availabe value returns std::nullopt
std::optional<T> tryPop();

// waits for available element and pushes
T waitAndPop();
```

- example:
```cpp
int main() {
  ReaderWriterQueue<int> queue(2); // maximum number of elements in queue : 2
  // asynchronous call of writer and reader
}

void writer(ReaderWriterQueue<int>& queue) {
  queue.tryPush(1); // won't return false because queue size is 2
  queue.tryPush(2); // -||-
  // queue.tryPush(2); could return false if reader haven't popped anythin yet
  queue.waitAndPush(3); // will wait for reader to pop anything if not popped alreadyu
}

void reader(ReaderWriterQueue<int>& queue) {
  int a = queue.waitAndPop(); // will wait for writer to push anything if not pushed already
  int b = queue.waitAndPop(); // -||-
  auto c = queue.tryPop();    // c may be nullopt, because writer may not have pushed everything
}
```

---

### SampleRateConversion


- there are available to functions for sample rate conversion:
```cpp


// out needs to have enough space, probably better to use the SoundBuffer returning versions, unless allocation is not wanted
void convertSampleRateLinearInterpolation(std::span<const sample_t> in, std::span<sample_t> out, Frequency inSampleRate, Frequency outSampleRate);
void convertSampleRateLinearInterpolation(std::span<const sample_t> in, std::span<sample_t> out, size_t inSize, size_t outSize);

// converts sample rate with linear interpolation and returns SoundBuffer with new sample rate
SoundBuffer convertSampleRateLinear(const SoundBuffer& in, Frequency outSampleRate);

// converts sample rate with sinc interpolation and returns SoundBuffer with new sample rate, if interrupt is not null, setting it to true will cancel operations
SoundBuffer convertSampleRateSinc(const SoundBuffer& in, Frequency outSampleRate, int32_t filterRadius = SampleRateConversion::DefaultFilterRadius, std::atomic_bool* interrupt = nullptr);

// change tempo of sound file, uses sinc interpolation, if interrupt is not null, setting it to true will cancel operations
SoundBuffer changeTempo(const SoundBuffer& in, double tempo, int32_t fitlerRadius = SampleRateConversion::DefaultFilterRadius, std::atomic_bool* interrupt = nullptr);

```

---

SampleRateConverter:

- it lets convert sample rate online.
- uses sinc filter

---

- to consruct:
```cpp
// inSampleRate  - sample rate of input
// outSampleRate - wanted sampleRate of outoput
// filterLength  - length of sinc filter, longer = better but slower
SampleRateConverter(Frequency inSampleRate_p, Frequency outSampleRate_p, size_t filterLength = 101)
```
---

- to process push samples with push(), check if availabe with outReady(), get samples with get()
```cpp
void push(sample_t in);
bool outReady();
sample_t get();
```

- example:
```cpp
// possible implementation of convertSampleRateSinc
void sampleRateConverterExample(std::span<const sample_t> in, std::span<sample_t> out, Frequency inSampleRate, Frequency outSampleRate) {
  using namespace ZAudio::Tools;

  // if no need to convert do not convert
  if(inSampleRate == outSampleRate) {
    for(size_t i = 0; i < std::min(in.size(), out.size()); i++) {
      out[i] = in[i];
    }
  }
  else {
    SampleRateConverter converter(inSampleRate, outSampleRate);
    size_t j = 0;
    for(size_t i =  0; i < in.size() && j < out.size(); i++) {
      converter.push(in[i]);

      // there can be multiple out samples from single push!
      while(converter.outReady() && j < out.size()) {
        out[j] = converter.get();
        j++;
      }
    }
  }
}
```

---

### LowFrequencyOscillator
LowFrequencyOscillator generates wave

- LowFrequencyOscillator::ShapeType:
```cpp
enum struct ShapeType {
  Saw, Sine, Triangle
};
```

- api:
```cpp
LowFrequencyOscillator();

// frequency_p - frequency of wave
// phaseOffset <0, 1) offset of wave for example sin with offset 0.5 is -sin
LowFrequencyOscillator(Frequency sampleRate_p, Frequency frequency_p, ShapeType shapeType_p, double phaseOffset = 0.);

void setFrequency(Frequency frequency_p);

void setShape(ShapeType shapeType_p);

// returns NormalizedValue - single sample of wave
NormalizedValue get();
```

- example:
```cpp
void lfoExample() {
  Tools::LowFrequencyOscillator lfo(Frequency::Hz(20), Frequency::Hz(2), Tools::LowFrequencyOscillator::ShapeType::Triangle);
  Tools::LowFrequencyOscillator ofsetedLfo(Frequency::Hz(20), Frequency::Hz(2), Tools::LowFrequencyOscillator::ShapeType::Triangle, 0.5);

  for(int i = 0; i < 10; i++) {
    std::cout << "lfo: " << lfo.get().bind(0., 1.) << "\n";
    std::cout << "ofsetedLfo: " << ofsetedLfo.get().bind(0., 1.) << "\n";
  }
}
```


- output:
```
lfo: 1
ofsetedLfo: 0
lfo: 0.8
ofsetedLfo: 0.2
lfo: 0.6
ofsetedLfo: 0.4
lfo: 0.4
ofsetedLfo: 0.6
lfo: 0.2
ofsetedLfo: 0.8
lfo: 0
ofsetedLfo: 1
lfo: 0.2
ofsetedLfo: 0.8
lfo: 0.4
ofsetedLfo: 0.6
lfo: 0.6
ofsetedLfo: 0.4
lfo: 0.8
ofsetedLfo: 0.2
```

---

### ModulatedDelay
ModulatedDelay is delay that varies its delay duration, it is used in for example Flanger and Chorus

- ModulatedDelay::Parameters:
```cpp

// rate of lfo
Frequency rate;

// delay duration range
Time minDelay;
Time maxDelay;

// gain of processed signal
Volume wet;

// gain of unprocessed signal
Volume dry;

// delay feedback
Volume feedback;

// see SimpleDelay
Time reservedDelay

// lfo shape type
LowFrequencyOscillator::ShapeType shape = LowFrequencyOscillator::ShapeType::Sine;

// must be <0., 1.>, optionally used to make less modulation, for example depth 0.5 will make modulation from minDelay to middle between minDelay and maxDelay
double depth = 1.;

// starting offset of lfo
double phaseOffset = 0.;
```

- api:
```cpp
ModulatedDelay();
ModulatedDelay(Frequency sampleRate_p, Parameters params);
void setRate(Frequency rate);
void setMinDelay(Time time);
void setMaxDelay(Time time);
void setWet(Volume wet);
void setDry(Volume dry);
void setFeedback(Volume feedback);
void setShape(LowFrequencyOscillator::ShapeType shape);
void setDepth(double depth_p);

// process sample and returns result
sample_t process(sample_t in);

// returns tail time in number of samples
uint32_t getTailTime() const;
```

- example:
```cpp
std::vector<sample_t> modulatedDelayExample(std::span<const sample_t> buffer, Frequency sampleRate) {
  Tools::ModulatedDelay::Parameters parameters;
  parameters.depth = 1.;
  parameters.dry = Volume::dB(0);
  parameters.wet = Volume::dB(-3);
  parameters.feedback = Volume::dB(-3);
  parameters.minDelay = Time::miliseconds(10);
  parameters.maxDelay = Time::miliseconds(100);
  parameters.phaseOffset = 0;
  parameters.rate = Frequency::Hz(1);
  parameters.reservedDelay = parameters.maxDelay;
  parameters.shape = Tools::LowFrequencyOscillator::ShapeType::Sine;
  Tools::ModulatedDelay md(sampleRate, parameters);

  // size of buffer + number of samples of echos
  std::vector<sample_t> ans(buffer.size() + md.getTailTime());
  for(size_t i = 0;  i < ans.size(); i++) {
    if(i < buffer.size()) {
      ans[i] = md.process(buffer[i]);
    }
    else {
      // already used buffer, but neeed to include echos
      ans[i] = md.process(0.);
    }
  }
  return ans;
}
```

---

### PhaseVocoder
PhaseVocoder uses windowed fft used for diffrent fft effects and pitchshifting

- api:
```cpp
// size_t frameSize is size of fft window
// size_t inputHopSize is offset of input frame
// size_t outputHopSize is offset of output frame
// PhaseCorrector::Algorithm is algorithm used in phasecorrector used with timestrecthing/shrinking

// constructs PhaseVocoder with input hop size, output hop size and phase correction algorithm, performs time stretching
PhaseVocoder(size_t frameSize_p, size_t inputHopSize_p, size_t outputHopSize_p, PhaseCorrector::Algorithm phaseCorrectionAlgorithm_p);

// constructs PhaseVocoder with just frameSize and hopSize no time stretching here
PhaseVocoder(size_t frameSize_p, size_t hopSize);

void push(sample_t in);

// if no time stretching should be called as often as push, before frame is ready it returns 0.
sample_t get();

// check if frame was just completed(after last push)
bool frameCompleted() const;

// returns reference to bins that can be changed before ifft
std::vector<std::complex<double>>& getBins();

void setInputHopSize(size_t inputHopSize_p);
void setOutputHopSize(size_t outputHopSize_p);
void setPhaseCorrectionAlgorithm(PhaseCorrector::Algorithm algorithm);
```

---

### PhaseCorrector
Used in PhaseVocoder provides few phase correction algorithms:
- Normal adjusts each bin phase independently
- Phase Lock - adjusts peaks and locks other bins to it
- Phase Track - phase lock but also tries to track moving peaks

- api:
```cpp
PhaseCorrector(size_t frameSize_p, size_t inputHopSize_p, size_t outputHopSize_p, Algorithm phaseCorrectionAlgorithm_p);

// corrects phases
void operator() (std::vector<std::complex<double>>& bins);

void setInputHopSize(size_t inputHopSize_p);
void setOutputHopSize(size_t outputHopSize_p);
void setAlgorithm(Algorithm algorithm);
```

---

### SimpleDelay
Delay that just adds delay with some time, no dry, wet or feedback.

  api :
```cpp
// sampleRate_p      - used sample rate
// delayTime         - duration of delay
// reservedDelayTime - / reserved time for delay, similiar as in std::vector, it can prevent allocations. Matters only when delay is changed
SimpleDelay(Frequency sampleRate_p, Time delayTime, Time reservedDelayTime);

sample_t get() const;
void push(sample_t in);
void setDelayTime(Time delayTime);

Time getDelayTime() const;
double getDelayInSamples() const; // return delay time in number of samples
```

---

### Smoother
Smoother is a class that can be used to make parameter changes smoother - change over time instead of instantly. It works with:
- double
- Time
- Frequency
- Volume
- To add other type, provide sepcilaziation to SmootherConverters:
- double toDouble(Type t);
- Type fromDouble(double v);

- to construct pass sampleRate, starting value and max change per second
```cpp
Smoother<T>(Frequency sampleRate, T from_p, T maxChangePerSecond);
```
---
- to change destination of smoothing (set value but slowly)
```cpp
void setDestination(T to_p);
```

---
- it is also possible to change value instantly without smoothing
```cpp
void setInstant(T to_p);
```

---
- it is important to call update every frame
```cpp
T update();
```
---
- to check if value is diffrent than frame before use:
```cpp
bool hasChanged() const;
```

---
- example:
```cpp

class MuterEffect {
public:
  MuterEffect(Frequency sampleRate) : smoother(sampleRate, Volume::dB(0.), Volume::db(1.)) {}

  sample_t process(sample_t in) {
    gain = smoother.update();
    // we could check if value has changed, like that :
    // if(valueSmoother.hasChanged()), but there is no reason to do so in this example
    return in * gain.linear();
  }

  void mute() {
    smoother.setDestination(Volume::linear(0.));
  }

  void unmute() {
    smoother.setDestination(Volume::linear(1.));
  }

private:
  Volume gain;
  Tools::Smoother<Volume> smoother;
};
```
### StringTools
StringTools have few functions that are usefull and not avaiable in std.


```cpp
// converts string to double, returns std::nullopt if string is not number
std::optional<double> stringToDouble(const std::string& str);

// converts string to int64_t, returns std::nullopt if string is integer
std::optional<int64_t> stringToInt(const std::string& str);

// checks if two string are case insensitive equal e.g. (abcd == AbCd)
bool caseInsensitiveEqual(const std::string& str1, const std::string& str2);
```

---

### ThreadTools
ThreadTools have (currently one) functions that are used for things with threads.
```cpp
void setHighPriority(std::thread& thread); // tries to set thread to be high priority
```

---

### TreeDatabase
TreeDatabase is class that is used to store some data in tree format (works like xml, but is made to easy change format if needed and add some abstraction level).
It stores tree with pairs (name - child) and values with pairs (name - value). There can only be one value and one child with same name in every node.
For example :

- Root
- - Child1 value1 = "123"  value2 = "1234"
- - Child2 value1 = "1234" value2 = "512"
- - - Child1


In xml:
```
<Root>

<Child1 value1 = "123" value2 = "1234">
</Child1>

<Child2 value1 = "1234" value2 = "512">

<Child1>
</Child1>
</Child1>

</Root>
```

- To create databse:
- - add child
```cpp
std::optional<DatabaseNodeID> addChild(DatabaseNodeID parent, const std::string& name)
```
- - if there is already child with given name or no node with id parent std::nullopt is returned
---
- - add value to some node
```cpp
template<typename T>
Result addValue(DatabaseNodeID node, const std::string& name, const T& value);

template<typename T, size_t N>
Result addEnumValue(DatabaseNodeID node, const std::string& name, T value, const std::array<std::pair<T, std::string>, N>& dictionary)
```
- - addValue adds pair to node (name - value), value should be convertible to string, being either fundamental type or implement std::string toString() const method.
- - if value with same name already exists or other error is found Result with desciription will be returned, else success
- - addEnumValue handles adding strong enums, it requires additional argument that specifies string value of each enum state in std::array
---

- To read from database
- - getChild - returns id of child with given name
```cpp
std::optional<DatabaseNodeID> getChild(DatabaseNodeID parent, const std::string& name) const;
```
- - returns nullopt if there is no node with parent id or there is no child with given name.
---
- - getValue from some node
```cpp
template<typename T, size_t N>
Result getEnumValue(DatabaseNodeID node, const std::string& name, T& value, const std::array<std::pair<T, std::string>, N>& dictionary) const

template<typename T>
Result getValue(DatabaseNodeID node, const std::string& name, T& v) const
```
- - getValue returns value(in v) associated with name, from node with ID node
- - return Result which is error when getValue failed
- - getEnum value also needs dictionary that specifies string value of each enum state in std::array

---

- export/import from file (currently only xml)
```cpp
template<typename In>
Result fromXML(In& in)

template<typename Out>
Result toXml(Out& out)
```
- -  fromXML loads database from file with stream in
- -  toXML saves database to file with stream out

---

- to get root id:
```cpp
DatabaseNodeID getRoot() const
```

---

There are classes that make it better to save recursively to database and only let read/write. They act as tree node.

TreeDatabaseWriter - only write operations
- api:
```cpp

// construct TreeDatabaseWriter that will act as its node_p
TreeDatabaseWriter(TreeDatabase* database_p, DatabaseNodeID node_p);

// add child to TreeDatabaseWriter node (returns other TreeDatabaseWriter to that child)
std::optional<TreeDatabaseWriter> addChild(const std::string& name) const;

// add value to TreeDatabaseWriter node
template<typename T>
Result addValue(const std::string& name, const T& value);

// add enum to TreeDatabaseWriter node
template<typename T, size_t N>
Result addEnumValue(const std::string& name, const T& value, const std::array<std::pair<T, std::string>, N>& dictionary);
```

TreeDatabaseReader - only read operations
- api:
```cpp

// construct TreeDatabaseReader that will act as its node_p
TreeDatabaseReader(const TreeDatabase* database_p, DatabaseNodeID node_p);

// gets child (returns other TreeDatabaseReader to that child)
std::optional<TreeDatabaseReader> getChild(std::string name) const;

// gets value from TreeDatabaseReader node
template<typename T>
Result getValue(std::string name, T& v);

// gets enum value from TreeDatabaseReader node
template<typename T, size_t N>
Result getEnumValue(std::string name, T& value, const std::array<std::pair<T, std::string>, N>& dictionary);
```

example:

```cpp
struct DatabaseExampleClassNested {
  float a;
  float b;

  Result load(TreeDatabaseReader reader) {
    Result r = Result::success();
    r &= reader.getValue("a", a);
    r &= reader.getValue("b", b);
    return r;
  }

  Result save(TreeDatabaseWriter writer) {
    Result r = Result::success();
    r &= writer.addValue("a", a);
    r &= writer.addValue("b", b);
    return r;
  }
};

struct DatabaseExampleClass {
  int a;
  int b;
  float c;
  float d;
  Volume vol;
  Frequency freq;
  Time time;
  DatabaseExampleClassNested nested;

  Result load(TreeDatabaseReader reader) {
    Result r = Result::success();
    r &= reader.getValue("a", a);
    r &= reader.getValue("b", b);
    r &= reader.getValue("c", c);
    r &= reader.getValue("d", d);
    r &= reader.getValue("vol", vol);
    r &= reader.getValue("freq", freq);
    r &= reader.getValue("time", time);

    auto child = reader.getChild("nestedClass");
    if(!child) {
      return r & Result::error("no nestClas child!");
    }

    r &= nested.load(*child);
    return r;
  }

  Result save(TreeDatabaseWriter writer) {
    Result r = Result::success();
    r &= writer.addValue("a", a);
    r &= writer.addValue("b", b);
    r &= writer.addValue("c", c);
    r &= writer.addValue("d", d);
    r &= writer.addValue("vol", vol);
    r &= writer.addValue("freq", freq);
    r &= writer.addValue("time", time);

    auto child = writer.addChild("nestedClass");
    if(!child) {
      return r & Result::error("cannot add nestedClass child!");
    }

    r &= nested.save(*child);
    return r;
  }
};

void databaseExample() {
  DatabaseExampleClass c;
  c.a = 1;
  c.b = 2;
  c.c = 3.5;
  c.d = -4.5;
  c.vol = Volume::dB(5);
  c.freq = Frequency::Hz(6);
  c.time = Time::miliseconds(7);
  c.nested = {.a = 8.1, .b = -8.1};

  TreeDatabase database;
  c.save(TreeDatabaseWriter(&database, database.getRoot()));
  database.toXml(std::cout);
}
```

- output:

```
<?xml version="1.0"?>
<xmlDoc time="0.007000s" vol="5.000000dB" c="3.500000" b="2" freq="6.000000hz" d="-4.500000" a="1">
        <nestedClass b="-8.100000" a="8.100000" />
</xmlDoc>
```

---

### WaveShapers
WaveShapers is set of wave shaping functions.
```cpp
enum struct WaveShaperType {
  Arraya, Sigmoid, Sigmoid2, HyperbolicTangent, ArcTangent, FuzzExponential1, FuzzExponential2, Exponential2, ArctangentSquareRoot,
  SquareSign, Cube, HardClipper, HalfWaveRectifier, FullWaveRectifier, SquareLaw, AbsoluteSquareRoot,
};


sample_t arraya(sample_t x);
sample_t sigmoid(sample_t x, sample_t k);
sample_t sigmoid2(sample_t x);
sample_t hyperbolicTangent(sample_t x, sample_t k);
sample_t arcTangent(sample_t x, sample_t k);
sample_t fuzzExponential1(sample_t x, sample_t k);
sample_t fuzzExponential2(sample_t x);
sample_t exponential2(sample_t x);
sample_t arctangetSquareRoot(sample_t x);
sample_t squareSign(sample_t x);
sample_t cube(sample_t x);
sample_t hardClipper(sample_t x);
sample_t halfWaveRectifier(sample_t x);
sample_t fullWaveRectifier(sample_t x);
sample_t squareLaw(sample_t x);
sample_t absoluteSquareRoot(sample_t x);

sample_t process(WaveShaperType type, sample_t x, sample_t k = 1.); // processes sample with choosen wave shaper
```

---

### WindowFunction
WindowFunction is class that provides some windowing functions, that are used for example in fft processing or sinc low pass.

- WindowFunction::Type - some popular types of windows
```cpp
enum struct Type {
  None, Hann, Hamming, Blackman
};
```

- api:
```cpp
static bool isMiddleSymmetric(Type type);                         // returns true if window(x) == window(-x)
static double get(Type type, double x);                           // returns value of window in specified x
static double get(Type type, size_t i, size_t size);              // returns value of window in specified x = i / size
static void calculateWindow(Type type, std::span<double> window); // calcualtes window of given type and saves it into std::span<double> window
WindowFunction(size_t size, Type type);                           // constructs WindFunction object with given window type
void applyWindow(std::span<sample_t> samples) const;              // applies window to samples
void applyWindow(std::span<sample_t> samples) const;              // applies window to samples (just multiplies each sample with each window value)
void applyGainCorrection(std::span<sample_t> samples) const       // applies gain correction to samples (multiplies each sample with gain correction value that is used to correct window gain change)
```

---

## Examples

There are many small examples in example.cpp and there is also a bit bigger example - CmdPlayer.


---

PortAudioIO, FileIO and SDL_IO are optional if you don't want to use them just don't add them to your project.

## Types

### sample_t

sample_t is type that is used for storing samples in whole library, it is defined as double

---

### FrameFormat
FrameFormat represents frame formats, currently only supported are: mono and stereo.

```cpp
enum struct FrameFormat {
  None, Mono, Stereo
};
```

---

There are also helper functions that can be used to FrameFormat or when manipulating frames:

- to get number of channels:
```cpp
size_t Tools::numberOfChannels(FrameFormat format);
```

- to convert one frame to other:
```cpp
void convertFrames(std::span<const sample_t> in, FrameFormat inFormat, std::span<sample_t> out, FrameFormat outFormat);
```

- to get maximum number of channels currently 2, because Stereo has 2 channels, there is static constexpr:
```
static constexpr inline size_t MaxNumberOfChannels = 2;
```

---

### SoundBuffer
SoundBuffer is type used to store sound and its information - sampleRate, frameFormat

- api:
```cpp

// construct buffer with sampleRate, frameFormat and lengthm, samples are filled with 0.
SoundBuffer(Frequency sampleRate_p, FrameFormat frameFormat_p, size_t length_p);

// construct buffer with sampleRate, frameFormat, loopStart(inSamples), loopEnd(inSamples), and length, samples are filled with 0.
SoundBuffer(Frequency sampleRate_p, FrameFormat frameFormat_p, size_t length_p, size_t loopStart_p, size_t loopEnd_p);

// copy values from begin end to frame with index x
template <typename It>
void setFrame(size_t x, It begin, It end);

// copy values from frame with index x to it
template<typename It>
void getFrame(size_t x, It it) const;

void setSample(size_t x, size_t channel, sample_t sample);
sample_t getSample(size_t x, size_t channel, sample_t sample) const;

FrameFormat getFrameFormat() const;
Frequency getSampleRate() const;
size_t getLength() const;
size_t getNumberOfChannels();
size_t getLoopStart() const;
size_t getLoopEnd() const;

// returns channel span (for exampe for stereo getChannel(0) returns left channel samples)
std::span<const sample_t> getChannel(size_t channel) const; 
std::span<sample_t> getChannel(size_t channel);
```

- example:
```cpp

SoundBuffer generateSoundBufferSine(Frequency sampleRate, Time duration, Frequency sineFreq) {
  SoundBuffer buffer(sampleRate, FrameFormat::Mono, duration.seconds() * sampleRate.Hz());

  double step = 2. * M_PI / (sampleRate.Hz() * sineFreq.Hz());
  double p = 0.;
  for(size_t i = 0; i < buffer.getLength(); i++) {
    buffer.setSample(i, 0, sin(p));
    p += step;
  }

  return buffer;
}

```

---

### Time

```cpp
//public api:
constexpr static Time minutes(double seconds_p);         // create Time with value in minuts
constexpr static Time seconds(double seconds_p);         // create Time with value in seconds
constexpr static Time miliseconds(double miliseconds_p); // create Time with value in miliseconds
constexpr static Time microseconds(double microseconds); // create Time with value in microseconds
constexpr double minutes() const;                        // get time in minutes
constexpr double seconds() const;                        // get time in seconds
constexpr double miliseconds() const;                    // get time in miliseconds
constexpr double microseconds() const;                   // get time in microseconds

// it also handles comprasions and (multiplication and divison with doubles)

std::string toString() const;
static std::optional<Time> fromString(const std::string& str);
```

- example:

```cpp
void timeExample() {
  using namespace ZAudio;

  std::cout << "Time = second(3.5)\n";
  Time time = Time::seconds(3.5);
  std::cout << "Time in seconds " << time.seconds() << "\n";
  std::cout << "Time in miliseconds " << time.miliseconds() << "\n";
  std::cout << "Time in microseconds " << time.microseconds() << "\n";
  std::cout << "\n";

  std::cout << "Time = milisecond(11)\n";
  time = Time::miliseconds(11);
  std::cout << "Time in seconds " << time.seconds() << "\n";
  std::cout << "Time in miliseconds " << time.miliseconds() << "\n";
  std::cout << "Time in microseconds " << time.microseconds() << "\n";
  std::cout << "\n";

  std::cout << "seconds(0.3) * 2.5 " << (Time::seconds(0.3) * 2.5).seconds() << "\n";
  std::cout << "seconds(0.3) / 2. " << (Time::seconds(0.3) / 2.).seconds() << "\n";
}
```

- output:

```
Time = second(3.5)
Time in seconds 3.5
Time in miliseconds 3500
Time in microseconds 3.5e+06

Time = milisecond(11)
Time in seconds 0.011
Time in miliseconds 11
Time in microseconds 11000

seconds(0.3) * 2.5 0.75
seconds(0.3) / 2. 0.15
```

---

### Volume
Volume is used to store relative volume (for example gain).

```cpp
// public api:
static Volume dB(double v);                // create Volume with db value
constexpr static Volume linear(double v);  // create Volume with linear value
double dB() const;                         // get db value
double linear() const;                     // get linear value
bool operator < (const Volume& oth) const;
bool operator > (const Volume& oth) const;
std::string toString() const;
static std::optional<Volume> fromString(const std::string& str);
```

- example:
```cpp
void volumeExample() {
  using namespace ZAudio;

  std::cout << "vol = 0 db\n";
  Volume vol = Volume::dB(0);

  std::cout << "linear: " << vol.linear() << "\n";

  std::cout << "vol = 3 db\n";
  vol = Volume::dB(3);

  std::cout << "linear: " << vol.linear() << "\n";

  std::cout << "vol = -3 db\n";
  vol = Volume::dB(-3);

  std::cout << "linear: " << vol.linear() << "\n";

  std::cout << "vol = 1 linear\n";
  vol = Volume::linear(1.);

  std::cout << "db: " << vol.dB() << "\n";
}
```

- output:
```
vol = 0 db
linear: 1
vol = 3 db
linear: 1.99526
vol = -3 db
linear: 0.501187
vol = 1 linear
db: 0
```
---

### Frequency

```cpp
// public api:
constexpr static Frequency Hz(double v);  // creates Frequencyh with value in hz
constexpr static Frequency KHz(double v); // creates Frequencyh with value in khz
constexpr double Hz() const;              // get frequency value in hz
constexpr double KHz() const;             // get frequency value in khz
constexpr Time getPeriod() const;
std::string toString() const;
static std::optional<Frequency> fromString(const std::string& str);
```

- example:
```cpp
void frequencyExample() {
  using namespace ZAudio;

  Frequency f = Frequency::Hz(155.5);
  std::cout << "f = 155.5 hz\n";

  std::cout << "khz: " << f.KHz() << "\n";

  f = Frequency::KHz(1.2);
  std::cout << "f = 1.2 khz\n";

  std::cout << "hz: " << f.Hz() << "\n";

  f = Frequency::Hz(5);
  std::cout << "f = 5 hz\n";

  std::cout << "period = " << f.getPeriod().seconds();
}
```

- output:
```
void frequencyExample() {
  using namespace ZAudio;

  Frequency f = Frequency::Hz(155.5);
  std::cout << "f = 155.5 hz\n";

  std::cout << "khz: " << f.KHz() << "\n";

  f = Frequency::KHz(1.2);
  std::cout << "f = 1.2 khz\n";

  std::cout << "hz: " << f.Hz() << "\n";

  f = Frequency::Hz(5);
  std::cout << "f = 5 hz\n";

  std::cout << "period = " << f.getPeriod().seconds();
}
```

---

### ParameterValue
ParameterValue can store Volume, Time, Frequency, int, double, bool and enum struct, it is used to pass values to and from effects, inputs and outputs.

```cpp
// public api:
/*implicit*/ ParameterValue(Frequency frequency) : floating(frequency.Hz());
/*implicit*/ ParameterValue(Volume volume) : floating(volume.linear());
/*implicit*/ ParameterValue(Time time) : floating(time.seconds());
/*implicit*/ ParameterValue(int32_t i) : floating(i);
/*implicit*/ ParameterValue(bool b) : floating(b ? 2. : 0.);
/*implicit*/ ParameterValue(double v) : floating(v);

template<typename T>
ParameterValue(T v); // from strong enum

Frequency getFrequency() const;

Volume getVolume() const;

Time getTime() const;

int getInteger() const;

double getDouble() const;

bool getBool() const;

template<typename T>
T getEnum() const;
```

---
### Result
Result is used to represent some error or success.

```cpp
// public api:
Result(bool error_p, std::string description_p); // constructs from error flag and description
static Result error(std::string description_p);  // creates error Result with description
static Result success();                         // creater success Result
Result operator & (const Result& oth);           // combines two Results - if at last one is error result is error
void operator &= (const Result& oth);            // combines this with oth
explicit operator bool() const;
std::string getDescription() const;
```

- example:
```cpp
void resultExample() {
  using namespace ZAudio;
  std::cout << std::boolalpha;

  Result result = Result::success();
  std::cout << "result = success\n";
  std::cout << "Result to bool is: " << static_cast<bool>(result) << "\n";

  result = Result::error("Some very bad things happend!");
  std::cout << "resutl = error\n";
  std::cout << "Result to bool is: " << static_cast<bool>(result) << "\n";
  std::cout << "Result description is: " << result.getDescription() << "\n";

  result &= Result::success();
  std::cout << "result &= succes but still value is: " << static_cast<bool>(result) << "\n";


  Result err1 = Result::error("very");
  Result err2 = Result::error("bad");
  Result err3 = Result::success();
  std::cout << "err1 & err2 & err3: to bool: " << static_cast<bool>(err1 & err2 & err3) << "\n";
  std::cout << "err1 & err2 & err3: description: " << (err1 & err2 & err3).getDescription() << "\n";
}
```

- output:
```
resutl = success
Result to bool is: true
resutl = error
Result to bool is: false
Result description is: Some very bad things happend!
result &= succes but still value is: false
err1 & err2 & err3: to bool: false
err1 & err2 & err3: description: very | bad
```

---


### ResultValue
ResultValue is similiar to Result, but it holds value(ResultValue<T> holds T value). It is usefull for returning values from functions that may fail.

```cpp
// public api:

// construct from value and result
ResultValue(T&& value_p, Result result_p);
ResultValue(const T& value_p, Result result_p);

// construct from just value - success
/*implicit*/ ResultValue(T&& value_p);
/*implicit*/ ResultValue(const T& value_p);

// construct from just Result - value is default constructed
/*implicit*/ ResultValue(Result result_p);

explicit operator bool() const;
std::string getDescription() const;

T& get();             // returns reference to underlying value
const T& get() const; // returns const reference to underlying value
```

- example:
```cpp

using namespace ZAudio;

ResultValue<int> functionThatMayFail(std::string str, int v) {
  if(str == ":)") {
    return 30 * v;
  }
  else {
    return Result::error("Why not smiling?");
  }
}

void resultValueTest() {
  std::cout << std::boolalpha;
  auto res = functionThatMayFail(":)", 2);
  std::cout << "to bool: " << static_cast<bool>(res) << "\n";
  std::cout << "value: " << res.get() << "\n";

  res = functionThatMayFail(":(", 1);
  std::cout << "to bool: " << static_cast<bool>(res) << "\n";
}

```

- output:

```
to bool: true
value: 60
to bool: false
```


## Dependencies

### dr_libs
Public domain, single file audio decoding libraries for C and C++.
https://github.com/mackron/dr_libs

used in FileIO, build from source, placed in ZAudio_FileIO folder

---

### PortAudio
PortAudio is a free, cross-platform, open-source, audio I/O library.
http://www.portaudio.com/

used in PortAudioIO, build from source, placed in PortAudioIO folder

---

### SDl3
Simple DirectMedia Layer is a cross-platform development library designed to provide low level access to audio, keyboard, mouse,
https://www.libsdl.org/

used in ZAudio_SDL_IO build from source, placed in ZAudio_SDLIO folder

---

### pugi xml
Light-weight, simple and fast XML parser for C++ with XPath support
https://pugixml.org/

used in TreeDatabase, build from source, placed in external folder


---

### fftw
FFTW is a C subroutine library for computing the discrete Fourier transform (DFT) in one or more dimensions, of arbitrary input size, and of both real and complex data (as well as of even/odd data, i.e. the discrete cosine/sine transforms or DCT/DST).
https://www.fftw.org/

used in FFT class, build from source, placed in external folder


## License

This library uses MIT license.

But there are included other libraries that use other licenses see [Dependencies](#dependencies).

---
---
---
