#pragma once

#include <thread>
#include <atomic>
#include <variant>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/Effect.h>
#include <ZAudio/BypassEffect.h>
#include <ZAudio/AudioInput.h>
#include <ZAudio/AudioOutput.h>
#include <ZAudio/ReaderWriterQueue.h>
#include <ZAudio/CircularBuffer.h>
#include <ZAudio/ThreadTools.h>

namespace ZAudio {


class Mixer; // forward
class AudioEngine; // forward
class AudioEngineInput; // forward
class AudioEngineOutput; // forward

template<typename T, typename ID>
class THandleID {
public:
  THandleID() = default;  

  bool operator == (const THandleID& oth) const {
    return ptr == oth.ptr && id == oth.id;
  }
private:
  friend Mixer;
  friend AudioEngine;
  friend AudioEngineInput;
  friend AudioEngineOutput;

  std::shared_ptr<T> ptr;  
  ID id;

  THandleID(ID id_p, std::shared_ptr<T> ptr_p) :
    id(id_p),
    ptr(ptr_p) {}

  T& get() {
    return *ptr;
  }

  const T& get() const {
    return *ptr;
  }
};

template<typename T>
class THandle {
public:
  THandle() = default;  

  bool operator == (const THandle& oth) const {
    return ptr == oth.ptr;
  }
private:
  friend Mixer;
  friend AudioEngine;
  std::shared_ptr<T> ptr;    

  THandle(std::shared_ptr<T> ptr_p) :
    ptr(ptr_p) {}

  T& get() {
    return *ptr;
  }

  const T& get() const {
    return *ptr;
  }
};

template<typename Phantom>
class TAudioEngineID {
public:
  TAudioEngineID() = default;
  explicit TAudioEngineID(uint64_t id_p) :
    id(id_p) {}

  uint64_t get() const {
    return id;  
  }

  bool operator == (TAudioEngineID oth) const {
    return id == oth.id;
  }
private:
  uint64_t id = 0;
};

using AudioEngineInputID = TAudioEngineID<class AudioEngineInputIDPhantom>;
using AudioEngineOutputID = TAudioEngineID<class AudioEngineOutputIDPhantom>;

using InputHandle = THandleID<AudioInput, AudioEngineInputID>;
using OutputHandle = THandleID<AudioOutput, AudioEngineOutputID>;
using EffectHandle = THandle<Effect>;
using MixerHandle = THandle<Mixer>;


} // namespace ZAudio

template <typename T>
struct std::hash<ZAudio::TAudioEngineID<T>> {
  std::size_t operator()(const ZAudio::TAudioEngineID<T>& k) const {
    return std::hash<uint64_t>()(k.get());
  }
};

namespace ZAudio {


class AudioEngineInput {
public:
  AudioEngineInput() = default;
  AudioEngineInput(InputHandle handle_p);

  void get(std::span<sample_t> out);
  void resetCached();

  AudioInput& getInput();
  int32_t getUseCount() const;
  void incrementUseCount();
  void decrementUseCount();

  bool died() const;
  bool notUsed() const;
private:
  InputHandle handle;
  int32_t useCount = 0;

  std::array<sample_t, Tools::MaxNumberOfChannels> cachedFrame;
  bool cached = false;
};

class AudioEngineOutput {
public:
  AudioEngineOutput() = default;
  AudioEngineOutput(OutputHandle handle_p);

  void send(std::span<const sample_t> out);
  void finishedFrame();

  OutputHandle& getOutput();
  int32_t getUseCount() const;
  void incrementUseCount();
  void decrementUseCount();

  bool notUsed() const;
private:
  OutputHandle handle;
  int32_t useCount = 0;

  std::array<sample_t, Tools::MaxNumberOfChannels> cachedFrame;
};


class Mixer {
public:
  Mixer(FrameFormat format_p, std::unordered_map<AudioEngineInputID, AudioEngineInput>* inputs_p);
  Mixer(EffectHandle effect_p, std::unordered_map<AudioEngineInputID, AudioEngineInput>* inputs_p);

  void setEffect(EffectHandle effect_p);

  void add(AudioEngineInputID input, EffectHandle effect_p);
  void stop(AudioEngineInputID input);
  void get(std::span<sample_t> out);
  bool errorOccured() const;
  bool isPlaying() const;
  FrameFormat getFormat() const;

private:      
  std::unordered_map<AudioEngineInputID, AudioEngineInput>* inputs;
  FrameFormat format;

  struct MixerInput {
    AudioEngineInputID input;
    EffectHandle effect;
    bool playing = true;
    uint32_t timeRemaining = 0;
  };

  struct TailEffect {
    EffectHandle effect;
    uint32_t timeRemaining = 0;
  };

  std::vector<MixerInput> playing;
  std::vector<TailEffect> tails;  

  EffectHandle mixerEffect;
  bool error = false;
};


class AudioEngine {
public:  
  AudioEngine(Frequency sampleRate_p);
  ~AudioEngine();

  MixerHandle addMixer(FrameFormat format);
  MixerHandle addMixer(EffectHandle effect);
  void addMixerOutput(const MixerHandle& input, const OutputHandle& output);
  void removeMixerOutput(const MixerHandle& mixer, const OutputHandle& output);
  void setMixerEffect(const MixerHandle& mixer, const EffectHandle& effect);
  void play(const MixerHandle& mixer, const InputHandle& input, const EffectHandle& effect);
  void play(const MixerHandle& mixer, const InputHandle& input);
  void stop(const MixerHandle& mixer, const InputHandle& input);
  EffectHandle addEffect(std::unique_ptr<Effect> effect);
  void setEffectParameter(const EffectHandle& handle, size_t parameterID, const ParameterValue& v);
  void setMultiEffectParameter(const EffectHandle& handle, size_t effectID, size_t parameterID, const ParameterValue& v);
  InputHandle addInput(std::unique_ptr<AudioInput> input);
  void setInputParameter(const InputHandle& handle, size_t parameterID, const ParameterValue& v );
  OutputHandle addOutput(std::unique_ptr<AudioOutput> output);
  void setOutputParameter(const OutputHandle& handle, size_t parameterID, const ParameterValue& v);
  bool isPlaying(const InputHandle& handle);
  bool hasEnded(const OutputHandle& handle);
  ParameterValue getOutputValue(const InputHandle& handle, size_t id);
  ParameterValue getOutputValue(const OutputHandle& handle, size_t id);
  ParameterValue getOutputValue(const EffectHandle& handle, size_t id);

  template<typename T, typename... Args>
  EffectHandle addEffect(Args&&... args) {
    return addEffect(std::make_unique<T>(std::forward<Args>(args)...));    
  }

  template<typename T, typename... Args>
  InputHandle addInput(Args&&... args) {
    return addInput(std::make_unique<T>(std::forward<Args>(args)...));
  }  

  template<typename T, typename... Args>
  OutputHandle addOutput(Args&&... args) {
    return addOutput(std::make_unique<T>(std::forward<Args>(args)...));
  }

private:
struct Command {  
  enum struct Type {
    AddMixer,
    AddMixerOutput,
    SetMixerEffect,
    Play,
    Stop,
    AddEffect,
    AddInput,
    AddOutput,
    SetEffectParameter,
    SetInputParameter, 
    SetOutputParameter,       
    RemoveMixerOutput,
    SetMultiEffectParameter,
    AskIsPlaying,
    GetAudioInputOutputValue,
    GetAudioOutputOutputValue,
    GetEffectOutputValue,
    AskHasEnded
  };
  size_t ind1 = 0;
  size_t ind2 = 0;  
  std::variant<MixerHandle, InputHandle, OutputHandle, EffectHandle> handle;
  std::variant<MixerHandle, InputHandle, OutputHandle, EffectHandle, ParameterValue> value1;
  std::variant<MixerHandle, InputHandle, OutputHandle, EffectHandle, ParameterValue> value2;    
  Type type;
};  
  static constexpr uint32_t QueueSize = 256;
  static constexpr uint32_t OutQueueSize = 2;
  Tools::ReaderWriterQueue<Command> queue;
  Tools::ReaderWriterQueue<ParameterValue> outQueue;
  
  std::vector<MixerHandle> mixers;
  std::vector<EffectHandle> effects;

  std::unordered_map<AudioEngineInputID, AudioEngineInput> inputs;  
  std::unordered_map<AudioEngineOutputID, AudioEngineOutput> outputs;  
  uint64_t nextID = 0;

  std::vector<std::pair<MixerHandle, AudioEngineOutputID>> mixersOutputs;

  Frequency sampleRate;  

  std::atomic_bool run{true};
  std::atomic_bool ready{false};
  std::atomic_bool error{false};
  std::thread thread;

  void addMixer(Command& command);
  void addMixerOutput(Command& command);
  void removeMixerOutput(Command& command);
  void setMixerEffect(Command& command);
  void play(Command& command);
  void stop(Command& command);
  void addEffect(Command& command);
  void addInput(Command& command);
  void addOutput(Command& command);
  void setEffectParameter(Command& command);
  void setInputParameter(Command& command);
  void setOutputParameter(Command& command);
  void setMultiEffectParameter(Command& command);
  void askIsPlaying(Command& command);
  void getAudioInputOutputValue(Command& command);
  void getAudioOutputOutputValue(Command& command);
  void getEffectOutputValue(Command& command);
  void askHasEnded(Command& command);
  void handleCommand(Command& command);
  void engineThread();

  template<typename T>
  T getNextID() {
    return T(nextID++);
  }
};


} // namespace ZAudio