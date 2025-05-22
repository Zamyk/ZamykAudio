#include <ZAudio/AudioEngine.h>

#include <cassert>
#include <iostream>
#include <utility>

namespace ZAudio {


// AudioEngineInput-------------------------------------------------------------------------------------------


AudioEngineInput::AudioEngineInput(InputHandle handle_p) :
  handle(handle_p)
{
  std::fill(cachedFrame.begin(), cachedFrame.end(), 0.);
}

void AudioEngineInput::get(std::span<sample_t> out) {
  if(!cached) {
    handle.get().get(cachedFrame);
    cached = true;
  }
  std::copy(cachedFrame.begin(), cachedFrame.end(), out.begin());
}

void AudioEngineInput::resetCached() {
  cached = false;
}

AudioInput& AudioEngineInput::getInput() {
  return handle.get();
}

int32_t AudioEngineInput::getUseCount() const {
  return useCount;
}

void AudioEngineInput::incrementUseCount() {
  useCount++;
}

void AudioEngineInput::decrementUseCount() {
  useCount--;
}

bool AudioEngineInput::died() const {
  return !handle.get().isPlaying() && (handle.ptr.use_count() == 1);
}

bool AudioEngineInput::notUsed() const {
  return useCount == 0;
}

bool AudioEngineInput::isPlaying() const {
  return handle.get().isPlaying();
}

// AudioEngineOutput-------------------------------------------------------------------------------------------


AudioEngineOutput::AudioEngineOutput(OutputHandle handle_p) :
  handle(handle_p)
{
  std::fill(cachedFrame.begin(), cachedFrame.end(), 0.);
}


void AudioEngineOutput::send(std::span<const sample_t> out) {
  for(size_t i = 0; i < cachedFrame.size(); i++) {
    cachedFrame[i] += out[i];
  }
}

void AudioEngineOutput::finishedFrame() {
  handle.get().send(cachedFrame);
  std::fill(cachedFrame.begin(), cachedFrame.end(), 0.);
}

OutputHandle& AudioEngineOutput::getOutput() {
  return handle;
}

int32_t AudioEngineOutput::getUseCount() const {
  return useCount;
}

void AudioEngineOutput::incrementUseCount() {
  useCount++;
}

void AudioEngineOutput::decrementUseCount() {
  useCount--;
}

bool AudioEngineOutput::notUsed() const {
  return useCount == 0;
}

// Mixer-------------------------------------------------------------------------------------------------------

Mixer::Mixer(FrameFormat format_p, std::unordered_map<AudioEngineInputID, AudioEngineInput>* inputs_p) :
  inputs(inputs_p),
  format(format_p),
  mixerEffect(std::make_shared<BypassEffect>(format, format)) {}

Mixer::Mixer(EffectHandle effect_p, std::unordered_map<AudioEngineInputID, AudioEngineInput>* inputs_p) :
  inputs(inputs_p),
  format(effect_p.get().getOutputFormat()),
  mixerEffect(effect_p) {}

void Mixer::setEffect(EffectHandle effect_p) {
  mixerEffect = effect_p;
  format = mixerEffect.get().getOutputFormat();
}

void Mixer::add(AudioEngineInputID input, EffectHandle effect_p) {
  playing.push_back({input, effect_p});
  (*inputs)[input].incrementUseCount();
}

void Mixer::stop(AudioEngineInputID input) {
  for(size_t i = 0; i < playing.size(); i++) {
    if(playing[i].input == input) {
      std::swap(playing[i], playing.back());
      TailEffect tail;
      tail.effect = playing.back().effect;
      tail.timeRemaining = tail.effect.get().getTailTime();
      if(tail.timeRemaining) {
        tails.push_back(tail);
      }
      playing.pop_back();
      (*inputs)[input].decrementUseCount();
    }
  }
}

void Mixer::get(std::span<sample_t> out) {
  std::array<sample_t, Tools::MaxNumberOfChannels> frame1;
  std::array<sample_t, Tools::MaxNumberOfChannels> frame2;
  std::array<sample_t, Tools::MaxNumberOfChannels> frame3;
  std::fill(frame3.begin(), frame3.end(), 0.);


  // fill output frame from inputs
  for(auto& p : playing) {
    std::fill(frame1.begin(), frame1.end(), 0.);
    std::fill(frame2.begin(), frame2.end(), 0.);

    auto& input = (*inputs)[p.input];
    auto& effect = p.effect.get();

    input.get(frame1);

    if(input.getInput().errorOccured()) {
      error = true;
    }

    if(input.getInput().isPlaying() && p.playing == false) {
      p.playing = true;
    }

    if(!input.getInput().isPlaying()) {
      if(p.playing) {
        p.playing = false;
        p.timeRemaining = p.effect.get().getTailTime();
      }

      if(p.timeRemaining != 0) {
        p.timeRemaining--;
      }
    }

    // convert input format to effect input format and process
    Tools::convertFrames(frame1, input.getInput().getFormat(), frame2, effect.getInputFormat());
    effect.process(frame2, frame1);

    // convert to output format of mixer
    Tools::convertFrames(frame1, effect.getOutputFormat(), frame2, mixerEffect.get().getInputFormat());

    for(size_t i = 0; i < Tools::numberOfChannels(format); i++) {
      frame3[i] += frame2[i];
    }
  }

  // delete unused from playing
  for(int32_t i = 0; i < static_cast<int32_t>(playing.size()); i++) {
    if((*inputs)[playing[i].input].died() && playing[i].timeRemaining == 0) {
      (*inputs)[playing[i].input].decrementUseCount();
      std::swap(playing.back(), playing[i]);
      playing.pop_back();
      i--;
    }
  }

  // add tail sounds (only these that were stopped, paused are working autoamtically with playing)
  for(auto& tail : tails) {
    std::fill(frame1.begin(), frame1.end(), 0.);
    tail.effect.get().process(frame1, frame2);
    Tools::convertFrames(frame2, tail.effect.get().getOutputFormat(), frame1, mixerEffect.get().getInputFormat());
    for(size_t i = 0; i < Tools::numberOfChannels(format); i++) {
      frame3[i] += frame1[i];
    }
    tail.timeRemaining--;
  }

  // remove ended tails
  for(int32_t i = 0; i < static_cast<int32_t>(tails.size()); i++) {
    if(tails[i].timeRemaining == 0) {
      std::swap(tails.back(), tails[i]);
      tails.pop_back();
      i--;
    }
  }

  mixerEffect.ptr->process(frame3, out);
}

bool Mixer::errorOccured() const {
  return error;
}

bool Mixer::isPlaying() const {
  return playing.size();
}

FrameFormat Mixer::getFormat() const {
  return format;
}


// AudioEngine----------------------------------------------------------------------------------------------

AudioEngine::AudioEngine(Frequency sampleRate_p) :
  queue(QueueSize),
  outQueue(OutQueueSize),
  sampleRate(sampleRate_p),
  thread(&AudioEngine::engineThread, this)
{
  ThreadTools::setHighPriority(thread);
  ready = true;
}

AudioEngine::~AudioEngine() {
  run = false;
  thread.join();
}

MixerHandle AudioEngine::addMixer(FrameFormat format) {
  MixerHandle handle(std::make_shared<Mixer>(format, &inputs));
  Command command;
  command.type = Command::Type::AddMixer;
  command.handle = handle;
  queue.waitAndPush(command);
  return handle;
}

MixerHandle AudioEngine::addMixer(EffectHandle effect) {
  MixerHandle handle(std::make_shared<Mixer>(effect, &inputs));
  Command command;
  command.type = Command::Type::AddMixer;
  command.handle = handle;
  queue.waitAndPush(command);
  return handle;
}

void AudioEngine::addMixerOutput(const MixerHandle& input, const OutputHandle& output) {
  Command command;
  command.type = Command::Type::AddMixerOutput;
  command.handle = input;
  command.value1 = output;
  queue.waitAndPush(command);
}

void AudioEngine::removeMixerOutput(const MixerHandle& mixer, const OutputHandle& output) {
  Command command;
  command.type = Command::Type::RemoveMixerOutput;
  command.handle = mixer;
  command.value1 = output;
  queue.waitAndPush(command);
}

void AudioEngine::setMixerEffect(const MixerHandle& mixer, const EffectHandle& effect) {
  Command command;
  command.type = Command::Type::SetMixerEffect;
  command.handle = mixer;
  command.value1 = effect;
  queue.waitAndPush(command);
}

void AudioEngine::play(const MixerHandle& mixer, const InputHandle& input, const EffectHandle& effect) {
  Command command;
  command.type = Command::Type::Play;
  command.handle = mixer;
  command.value1 = input;
  command.value2 = effect;
  queue.waitAndPush(command);
}

void AudioEngine::play(const MixerHandle& mixer, const InputHandle& input) {
  Command command;
  command.type = Command::Type::Play;
  command.handle = mixer;
  command.value1 = input;
  command.value2 = EffectHandle(std::make_shared<BypassEffect>(input.get().getFormat(), mixer.get().getFormat()));
  queue.waitAndPush(command);
}

void AudioEngine::stop(const MixerHandle& mixer, const InputHandle& input) {
  Command command;
  command.type = Command::Type::Stop;
  command.handle = mixer;
  command.value1 = input;
  queue.waitAndPush(command);
}

EffectHandle AudioEngine::addEffect(std::unique_ptr<Effect> effect) {
  effect->setSampleRate(sampleRate);
  EffectHandle handle(std::move(effect));
  Command command;
  command.type = Command::Type::AddEffect;
  command.handle = handle;
  queue.waitAndPush(command);
  return handle;
}

void AudioEngine::setEffectParameter(const EffectHandle& handle, size_t parameterID, const ParameterValue& v) {
  Command command;
  command.type = Command::Type::SetEffectParameter;
  command.handle = handle;
  command.ind1 = parameterID;
  command.value1 = v;
  queue.waitAndPush(command);
}

void AudioEngine::setMultiEffectParameter(const EffectHandle& handle, size_t effectID, size_t parameterID, const ParameterValue& v) {
  Command command;
  command.type = Command::Type::SetMultiEffectParameter;
  command.handle = handle;
  command.ind1 = effectID;
  command.ind2 = parameterID;
  command.value1 = v;
  queue.waitAndPush(command);
}

InputHandle AudioEngine::addInput(std::unique_ptr<AudioInput> input) {
  input->setSampleRate(sampleRate);
  InputHandle handle(getNextID<AudioEngineInputID>(), std::move(input));
  Command command;
  command.type = Command::Type::AddInput;
  command.handle = handle;
  queue.waitAndPush(command);
  return handle;
}

void AudioEngine::setInputParameter(const InputHandle& handle, size_t parameterID, const ParameterValue& v ) {
  Command command;
  command.type = Command::Type::SetInputParameter;
  command.handle = handle;
  command.ind1 = parameterID;
  command.value1 = v;
  queue.waitAndPush(command);
}

OutputHandle AudioEngine::addOutput(std::unique_ptr<AudioOutput> output) {
  output->setSampleRate(sampleRate);
  OutputHandle handle(getNextID<AudioEngineOutputID>(), std::move(output));
  Command command;
  command.type = Command::Type::AddOutput;
  command.handle = handle;
  queue.waitAndPush(command);
  return handle;
}

void AudioEngine::setOutputParameter(const OutputHandle& handle, size_t parameterID, const ParameterValue& v) {
  Command command;
  command.type = Command::Type::SetOutputParameter;
  command.handle = handle;
  command.ind1 = parameterID;
  command.value1 = v;
  queue.waitAndPush(command);
}

bool AudioEngine::isPlaying(const InputHandle& handle) {
  Command command;
  command.type = Command::Type::AskIsPlaying;
  command.handle = handle;
  queue.waitAndPush(command);
  return outQueue.waitAndPop().getBoolean();
}

bool AudioEngine::hasEnded(const OutputHandle& handle) {
  Command command;
  command.type = Command::Type::AskHasEnded;
  command.handle = handle;
  queue.waitAndPush(command);
  return outQueue.waitAndPop().getBoolean();
}

ParameterValue AudioEngine::getOutputValue(const InputHandle& handle, size_t id) {
  Command command;
  command.type = Command::Type::GetAudioInputOutputValue;
  command.handle = handle;
  command.ind1 = id;
  queue.waitAndPush(command);
  return outQueue.waitAndPop();
}

ParameterValue AudioEngine::getOutputValue(const OutputHandle& handle, size_t id) {
  Command command;
  command.type = Command::Type::GetAudioOutputOutputValue;
  command.handle = handle;
  command.ind1 = id;
  queue.waitAndPush(command);
  return outQueue.waitAndPop();
}

ParameterValue AudioEngine::getOutputValue(const EffectHandle& handle, size_t id) {
  Command command;
  command.type = Command::Type::GetEffectOutputValue;
  command.handle = handle;
  command.ind1 = id;
  queue.waitAndPush(command);
  return outQueue.waitAndPop();
}

void AudioEngine::addMixer(Command& command) {
  mixers.push_back(std::get<MixerHandle>(command.handle));
}

void AudioEngine::addMixerOutput(Command& command) {
  auto& mixer = std::get<MixerHandle>(command.handle);
  auto& output = std::get<OutputHandle>(command.value1);
  outputs[output.id].incrementUseCount();
  mixersOutputs.push_back({mixer, output.id});
}

void AudioEngine::removeMixerOutput(Command& command) {
  auto& mixer = std::get<MixerHandle>(command.handle);
  auto& output = std::get<OutputHandle>(command.value1);
  outputs[output.id].decrementUseCount();
  mixersOutputs.erase(std::find(mixersOutputs.begin(), mixersOutputs.end(), std::make_pair(mixer, output.id)));
}

void AudioEngine::setMixerEffect(Command& command) {
  auto& mixer = *std::get<MixerHandle>(command.handle).ptr;
  auto effect = std::get<EffectHandle>(command.value1);
  mixer.setEffect(effect);
}

void AudioEngine::play(Command& command) {
  auto& mixer = *std::get<MixerHandle>(command.handle).ptr;
  auto& input = std::get<InputHandle>(command.value1);
  auto& effect = std::get<EffectHandle>(command.value2);
  mixer.add(input.id, effect);
}

void AudioEngine::stop(Command& command) {
  auto& mixer = *std::get<MixerHandle>(command.handle).ptr;
  auto& input = std::get<InputHandle>(command.value1);
  mixer.stop(input.id);
}

void AudioEngine::addEffect(Command& command) {
  effects.push_back(std::get<EffectHandle>(command.handle));
}

void AudioEngine::addInput(Command& command) {
  AudioEngineInput input(std::get<InputHandle>(command.handle));
  inputs.insert({std::get<InputHandle>(command.handle).id, input});
}

void AudioEngine::addOutput(Command& command) {
  AudioEngineOutput output(std::get<OutputHandle>(command.handle));
  outputs.insert({std::get<OutputHandle>(command.handle).id, output});
}

void AudioEngine::setEffectParameter(Command& command) {
  std::get<EffectHandle>(command.handle).ptr->setParameter(command.ind1, std::get<ParameterValue>(command.value1));
}

void AudioEngine::setInputParameter(Command& command) {
  std::get<InputHandle>(command.handle).get().setParameter(command.ind1, std::get<ParameterValue>(command.value1));
}

void AudioEngine::setOutputParameter(Command& command) {
  std::get<OutputHandle>(command.handle).get().setParameter(command.ind1, std::get<ParameterValue>(command.value1));
}

void AudioEngine::setMultiEffectParameter(Command& command) {
  std::get<EffectHandle>(command.handle).ptr->setParameter(command.ind1, command.ind2, std::get<ParameterValue>(command.value1));
}

void AudioEngine::askIsPlaying(Command& command) {
  outQueue.tryPush(ParameterValue::boolean(std::get<InputHandle>(command.handle).get().isPlaying() && (!inputs[std::get<InputHandle>(command.handle).id].notUsed())));
}

void AudioEngine::getAudioInputOutputValue(Command& command) {
  outQueue.tryPush(std::get<InputHandle>(command.handle).get().getOutputValue(command.ind1));
}

void AudioEngine::getAudioOutputOutputValue(Command& command) {
  outQueue.tryPush(std::get<OutputHandle>(command.handle).get().getOutputValue(command.ind1));
}

void AudioEngine::getEffectOutputValue(Command& command) {
  outQueue.tryPush(std::get<EffectHandle>(command.handle).get().getOutputValue(command.ind1));
}

void AudioEngine::askHasEnded(Command& command) {
  outQueue.tryPush(ParameterValue::boolean(std::get<OutputHandle>(command.handle).get().ended()));
}

void AudioEngine::handleCommand(Command& command) {
  switch(command.type) {
    case Command::Type::AddEffect:
      addEffect(command);
      break;

    case Command::Type::AddInput:
      addInput(command);
      break;

    case Command::Type::AddMixer:
      addMixer(command);
      break;

    case Command::Type::AddMixerOutput:
      addMixerOutput(command);
      break;

    case Command::Type::AddOutput:
      addOutput(command);
      break;

    case Command::Type::Play:
      play(command);
      break;

    case Command::Type::RemoveMixerOutput:
      removeMixerOutput(command);
      break;

    case Command::Type::SetEffectParameter:
      setEffectParameter(command);
      break;

    case Command::Type::SetInputParameter:
      setInputParameter(command);
      break;

    case Command::Type::SetMixerEffect:
      setMixerEffect(command);
      break;

    case Command::Type::SetOutputParameter:
      setOutputParameter(command);
      break;

    case Command::Type::Stop:
      stop(command);
      break;

    case Command::Type::SetMultiEffectParameter:
      setMultiEffectParameter(command);
      break;

    case Command::Type::AskIsPlaying:
      askIsPlaying(command);
      break;

    case Command::Type::GetAudioInputOutputValue:
      getAudioInputOutputValue(command);
      break;

    case Command::Type::GetAudioOutputOutputValue:
      getAudioInputOutputValue(command);
      break;

    case Command::Type::GetEffectOutputValue:
      getAudioInputOutputValue(command);
      break;

    default:
      assert(false);
  }
}


void AudioEngine::engineThread() {
  while(!ready) {
    std::this_thread::yield();
  }
  std::array<sample_t, Tools::MaxNumberOfChannels> frame1;
  std::array<sample_t, Tools::MaxNumberOfChannels> frame2;

  while(run) {
    while(auto command = queue.tryPop()) {
      handleCommand(*command);
    }
    //clean(mixers);
    //clean(effects);
    //clean(inputs);
    //clean(outputs);
    //clean(playbacks);
    std::fill(frame1.begin(), frame1.end(), 0.);
    std::fill(frame2.begin(), frame2.end(), 0.);

    for(auto& input : inputs) {
      input.second.resetCached();
    }

    for(auto it = inputs.begin(); it != inputs.end();) {
      if(it->second.getUseCount() == 0 && it->second.isPlaying() == false) {
        it = inputs.erase(it);        
      }
      else {
        ++it;
      }
    }


    for(auto& p : mixersOutputs) {
      p.first.ptr->get(frame1);
      if(p.first.ptr->errorOccured()) {
        error = true;
      }
      Tools::convertFrames(frame1, p.first.ptr->getFormat(), frame2, outputs[p.second].getOutput().get().getFormat());
      outputs[p.second].send(frame2);
      if(outputs[p.second].getOutput().get().errorOccured()) {
        error = true;
      }
    }

    for(auto& output : outputs) {
      output.second.finishedFrame();
    }
  }
}


} // namespace ZAudio
