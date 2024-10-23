#include <ZAudio/EffectSerializer.h>
#include <ZAudio/Effect.h>

#include <ZAudio/AutoWahEffect.h>
#include <ZAudio/BitCrusherEffect.h>
#include <ZAudio/BypassEffect.h>
#include <ZAudio/DelayEffect.h>
#include <ZAudio/DuckDelayEffect.h>
#include <ZAudio/DynamicsProcessorEffect.h>
#include <ZAudio/FilterEffect.h>
#include <ZAudio/FlangerEffect.h>
#include <ZAudio/LfoWahEffect.h>
#include <ZAudio/LooperEffect.h>
#include <ZAudio/MonoToStereoAdapter.h>
#include <ZAudio/PhaserEffect.h>
#include <ZAudio/PingPongDelayEffect.h>
#include <ZAudio/PitchShiftEffect.h>
#include <ZAudio/ReverseDelayEffect.h>
#include <ZAudio/RobotEffect.h>
#include <ZAudio/SequenceFilterEffect.h>
#include <ZAudio/StereoChorusEffect.h>
#include <ZAudio/StereoFlangerEffect.h>
#include <ZAudio/StereoPhaserEffect.h>
#include <ZAudio/TremoloEffect.h>
#include <ZAudio/TubePreampEffect.h>
#include <ZAudio/VibratoEffect.h>
#include <ZAudio/VolumeControlEffect.h>
#include <ZAudio/WhisperEffect.h>
#include <ZAudio/SerialEffect.h>
#include <ZAudio/ParallelEffect.h>
#include <ZAudio/Spatial2dEffect.h>

namespace ZAudio::Tools {


EffectSerializer::EffectSerializer() {    
  addEffectType(AutoWahEffect(AutoWahEffect::Parameters()));
  addEffectType(BitCrusherEffect(BitCrusherEffect::Parameters()));
  addEffectType(BypassEffect(FrameFormat::Mono, FrameFormat::Mono));  
  addEffectType(DelayEffect(DelayEffect::Parameters()));
  addEffectType(DuckDelayEffect(DuckDelayEffect::Parameters()));
  addEffectType(DynamicsProcessorEffect(DynamicsProcessorEffect::Parameters()));
  addEffectType(FilterEffect(FilterEffect::Parameters()));
  addEffectType(FlangerEffect(FlangerEffect::Parameters()));
  addEffectType(LfoWahEffect(LfoWahEffect::Parameters()));
  addEffectType(MonoToStereoAdapter());
  addEffectType(LooperEffect(LooperEffect::Parameters()));
  addEffectType(PhaserEffect(PhaserEffect::Parameters()));
  addEffectType(PingPongDelayEffect(PingPongDelayEffect::Parameters()));
  addEffectType(PitchShiftEffect(PitchShiftEffect::Parameters()));  
  addEffectType(ReverseDelayEffect(ReverseDelayEffect::Parameters()));
  addEffectType(RobotEffect(RobotEffect::Parameters()));
  addEffectType(SequenceFilterEffect(SequenceFilterEffect::Parameters()));
  addEffectType(StereoChorusEffect(StereoChorusEffect::Parameters()));
  addEffectType(StereoFlangerEffect(StereoFlangerEffect::Parameters()));  
  addEffectType(StereoPhaserEffect(StereoPhaserEffect::Parameters()));
  addEffectType(TremoloEffect(TremoloEffect::Parameters()));
  addEffectType(TubePreampEffect(TubePreampEffect::Parameters()));
  addEffectType(VibratoEffect(VibratoEffect::Parameters()));
  addEffectType(VolumeControlEffect(VolumeControlEffect::Parameters(Volume::dB(1.))));
  addEffectType(WhisperEffect(WhisperEffect::Parameters()));
  addEffectType(SerialEffect(1));
  addEffectType(ParallelEffect(FrameFormat::Mono, FrameFormat::Mono, 1));
  addEffectType(Spatial2dEffect(Spatial2dEffect::Parameters()));
}

Result EffectSerializer::save(TreeDatabaseWriter writer, const Effect& effect) {
  Result r = Result::success();
  r &= writer.addValue("EffectID", effect.getID());
  r &= writer.addValue("Version", effect.getVersion());
  auto parametersWriter = writer.addChild("Parameters");
  if(!parametersWriter) {
    return r & Result::error("Couldn't create child parameters");
  }
  r &= effect.save(*parametersWriter);
  return r;
}

ResultValue<std::unique_ptr<Effect>> EffectSerializer::load(TreeDatabaseReader reader) {
  std::string id;
  Result r = Result::success();
  r &= reader.getValue<std::string>("EffectID", id);  

  int64_t version;
  r &= reader.getValue<int64_t>("Version", version);
  
  if(!r) {
    return r;
  }

  auto effect = getType(id);
  if(!effect) {    
    return Result::error("No effect in base with id " + id);
  }    
  if(version != effect->getVersion()) {
    return Result::error("Version mismatch");
  }

  auto parameters = reader.getChild("Parameters");
  if(!parameters) {
    return Result::error("No child parameters");
  }

  r = effect->load(*parameters);

  if(!r) {
    return r;
  }

  return std::move(effect);
}

Result EffectSerializer::save(TreeDatabase& database, const Effect& effect) {
  auto node = database.addChild(database.getRoot(), "Effect");
  if(!node) {
    return Result::error("Couldn't get child Error");
  }
  return save(TreeDatabaseWriter(&database, *node), effect);
}

ResultValue<std::unique_ptr<Effect>> EffectSerializer::load(TreeDatabase& database) {
  auto node = database.getChild(database.getRoot(), "Effect");
  if(!node) {
    return Result::error("Couldn't get child Error");
  }
  return load(TreeDatabaseReader(&database, *node));
}

std::unique_ptr<Effect> EffectSerializer::getType(const std::string& id) {
  if(types.count(id) == 0) {
    return nullptr;
  }
  return types[id]->clone();
}

EffectSerializer& EffectSerializer::instance() {
  static EffectSerializer instance;
  return instance;
}

void EffectSerializer::addEffectType(const Effect& effectType) {
  types.insert({effectType.getID(), effectType.clone()});
}  


} // namespace ZAudio::Tools