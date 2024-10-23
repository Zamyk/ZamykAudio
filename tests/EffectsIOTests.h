#pragma once

#include "catch/catch.hpp"


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
#include <ZAudio/Spatial2dEffect.h>
#include <ZAudio/SerialEffect.h>
#include <ZAudio/ParallelEffect.h>

#include <sstream>

#ifdef DO_LATER
TEST_CASE("AutoWahEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  AutoWahEffect::Parameters parameters;
  parameters.decay = Time::seconds(1.);
  parameters.highEnvelope = Volume::dB(1.);
  parameters.lowEnvelope = Volume::dB(1.);
  parameters.maxFrequency = Frequency::Hz(1.);
  parameters.minFrequency = Frequency::Hz(1.);
  parameters.q = 1.;
  AutoWahEffect effect(parameters);
  effect.setParameter(AutoWahEffect::DecayID, Time::seconds(2.));
  effect.setParameter(AutoWahEffect::HighEnvelopeID, Volume::dB(3.));
  effect.setParameter(AutoWahEffect::LowEnvelopeID, Volume::dB(4.));
  effect.setParameter(AutoWahEffect::MaxFrequencyID, Frequency::Hz(5.));
  effect.setParameter(AutoWahEffect::MinFrequencyID, Frequency::Hz(6.));
  effect.setParameter(AutoWahEffect::Q_ID, 7.);
  auto tmp = effect.getParameters();
  
  REQUIRE_THAT(tmp.decay.seconds(), Catch::Matchers::WithinAbs(2., 0.0001));
  REQUIRE_THAT(tmp.highEnvelope.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.lowEnvelope.dB(), Catch::Matchers::WithinAbs(4., 0.0001));
  REQUIRE_THAT(tmp.maxFrequency.Hz(), Catch::Matchers::WithinAbs(5., 0.0001));
  REQUIRE_THAT(tmp.minFrequency.Hz(), Catch::Matchers::WithinAbs(6., 0.0001));
  REQUIRE_THAT(tmp.q, Catch::Matchers::WithinAbs(7., 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);  
  
  tmp = dynamic_cast<AutoWahEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.decay.seconds(), Catch::Matchers::WithinAbs(2., 0.0001));
  REQUIRE_THAT(tmp.highEnvelope.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.lowEnvelope.dB(), Catch::Matchers::WithinAbs(4., 0.0001));
  REQUIRE_THAT(tmp.maxFrequency.Hz(), Catch::Matchers::WithinAbs(5., 0.0001));
  REQUIRE_THAT(tmp.minFrequency.Hz(), Catch::Matchers::WithinAbs(6., 0.0001));
  REQUIRE_THAT(tmp.q, Catch::Matchers::WithinAbs(7., 0.0001));
}

TEST_CASE("BitCrusherEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  BitCrusherEffect::Parameters parameters;
  parameters.bitDepth = 1;
  parameters.dry = Volume::dB(1); 
  parameters.wet = Volume::dB(1);
  
  BitCrusherEffect effect(parameters);
  effect.setParameter(BitCrusherEffect::BitDepthID, 2.);
  effect.setParameter(BitCrusherEffect::DryID, Volume::dB(3));
  effect.setParameter(BitCrusherEffect::WetID, Volume::dB(4));
  auto tmp = effect.getParameters();
  
  REQUIRE_THAT(tmp.bitDepth, Catch::Matchers::WithinAbs(2., 0.0001));  
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(3., 0.0001));  
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(4., 0.0001));  

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);  
  
  tmp = dynamic_cast<BitCrusherEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.bitDepth, Catch::Matchers::WithinAbs(2., 0.0001));  
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(3., 0.0001));  
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(4., 0.0001));  
}

TEST_CASE("BypassEffect") {
  // TODO
/*
  using namespace ZAudio;
  using namespace ZAudio::Tools;      
  BypassEffect effect(FrameFormat::Stereo, FrameFormat::Mono);    
  REQUIRE(effect.getInputFormat() == FrameFormat::Stereo);
  REQUIRE(effect.getOutputFormat() == FrameFormat::Mono);  

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);  
  
  auto& tmp = dynamic_cast<BypassEffect&>(*loaded.v());

  REQUIRE(tmp.getInputFormat() == FrameFormat::Stereo);
  REQUIRE(tmp.getOutputFormat() == FrameFormat::Mono);  
*/
}

TEST_CASE("DelayEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;      
  DelayEffect::Parameters parameters;
  parameters.delayTime = Time::seconds(1);
  parameters.dry = Volume::dB(1);
  parameters.feedback = Volume::dB(1);
  parameters.reservedDelayTime = Time::seconds(1);
  parameters.wet = Volume::dB(1);  
  
  DelayEffect effect(parameters);
  effect.setParameter(DelayEffect::DelayTimeID, Time::seconds(2.));  
  effect.setParameter(DelayEffect::DryID, Volume::dB(3.));  
  effect.setParameter(DelayEffect::FeedbackID, Volume::dB(4.));  
  effect.setParameter(DelayEffect::ReservedDelayTimeID, Time::seconds(5.));  
  effect.setParameter(DelayEffect::WetID, Volume::dB(6.));  
  auto tmp = effect.getParameters();
  
  REQUIRE_THAT(tmp.delayTime.seconds(), Catch::Matchers::WithinAbs(2., 0.0001));
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(4., 0.0001));
  REQUIRE_THAT(tmp.reservedDelayTime.seconds(), Catch::Matchers::WithinAbs(5., 0.0001));
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(6., 0.0001));  

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);  
  
  tmp = dynamic_cast<DelayEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.delayTime.seconds(), Catch::Matchers::WithinAbs(2., 0.0001));
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(4., 0.0001));
  REQUIRE_THAT(tmp.reservedDelayTime.seconds(), Catch::Matchers::WithinAbs(5., 0.0001));
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(6., 0.0001));  
}

TEST_CASE("DuckDelayEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;      
  DuckDelayEffect::Parameters parameters;
  parameters.attackTime = Time::seconds(1);
  parameters.delayTime = Time::seconds(1);
  parameters.dry = Volume::dB(1);
  parameters.feedback = Volume::dB(1);
  parameters.recoveryTime = Time::seconds(1);
  parameters.releaseTime = Time::seconds(1);
  parameters.reservedDelayTime = Time::seconds(1);
  parameters.threshold = Volume::dB(1);
  parameters.wetMin = Volume::dB(1);
  parameters.wetMax = Volume::dB(1);
  
  DuckDelayEffect effect(parameters);
  effect.setParameter(DuckDelayEffect::AttackTimeID, Time::seconds(2));
  effect.setParameter(DuckDelayEffect::DelayTimeID, Time::seconds(3));
  effect.setParameter(DuckDelayEffect::DryID, Volume::dB(4));
  effect.setParameter(DuckDelayEffect::FeedbackID, Volume::dB(5));
  effect.setParameter(DuckDelayEffect::RecoveryTimeID, Time::seconds(6));
  effect.setParameter(DuckDelayEffect::ReleaseTimeID, Time::seconds(7));
  effect.setParameter(DuckDelayEffect::ReservedDelayTimeID, Time::seconds(8));
  effect.setParameter(DuckDelayEffect::ThresholdID, Volume::dB(9));
  effect.setParameter(DuckDelayEffect::WetMinID, Volume::dB(10));
  effect.setParameter(DuckDelayEffect::WetMaxID, Volume::dB(11));
  auto tmp = effect.getParameters();
  
  REQUIRE_THAT(tmp.attackTime.seconds(), Catch::Matchers::WithinAbs(2., 0.0001));  
  REQUIRE_THAT(tmp.delayTime.seconds(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(4., 0.0001));
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(5., 0.0001));
  REQUIRE_THAT(tmp.recoveryTime.seconds(), Catch::Matchers::WithinAbs(6., 0.0001));
  REQUIRE_THAT(tmp.releaseTime.seconds(), Catch::Matchers::WithinAbs(7., 0.0001));
  REQUIRE_THAT(tmp.reservedDelayTime.seconds(), Catch::Matchers::WithinAbs(8., 0.0001));
  REQUIRE_THAT(tmp.threshold.dB(), Catch::Matchers::WithinAbs(9., 0.0001));
  REQUIRE_THAT(tmp.wetMin.dB(), Catch::Matchers::WithinAbs(10., 0.0001));
  REQUIRE_THAT(tmp.wetMax.dB(), Catch::Matchers::WithinAbs(11., 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);  
  
  tmp = dynamic_cast<DuckDelayEffect&>(*loaded.get()).getParameters();

    
  REQUIRE_THAT(tmp.attackTime.seconds(), Catch::Matchers::WithinAbs(2., 0.0001));  
  REQUIRE_THAT(tmp.delayTime.seconds(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(4., 0.0001));
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(5., 0.0001));
  REQUIRE_THAT(tmp.recoveryTime.seconds(), Catch::Matchers::WithinAbs(6., 0.0001));
  REQUIRE_THAT(tmp.releaseTime.seconds(), Catch::Matchers::WithinAbs(7., 0.0001));
  REQUIRE_THAT(tmp.reservedDelayTime.seconds(), Catch::Matchers::WithinAbs(8., 0.0001));
  REQUIRE_THAT(tmp.threshold.dB(), Catch::Matchers::WithinAbs(9., 0.0001));
  REQUIRE_THAT(tmp.wetMin.dB(), Catch::Matchers::WithinAbs(10., 0.0001));
  REQUIRE_THAT(tmp.wetMax.dB(), Catch::Matchers::WithinAbs(11., 0.0001));
}

TEST_CASE("DynamicsProcessorEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;      
  DynamicsProcessorEffect::Parameters parameters;
  parameters.attackTime = Time::seconds(1);
  parameters.hardStop = true;
  parameters.outputGain = Volume::dB(1);
  parameters.ratio = 1;
  parameters.releaseTime = Time::seconds(1);
  parameters.threshold = Volume::dB(1);
  parameters.type = DynamicsProcessorEffect::Type::Compressor;
  
  DynamicsProcessorEffect effect(parameters);
  effect.setParameter(DynamicsProcessorEffect::AttackTimeID, Time::seconds(2.));
  effect.setParameter(DynamicsProcessorEffect::HardStopID, false);
  effect.setParameter(DynamicsProcessorEffect::OutputGainID, Volume::dB(3.));
  effect.setParameter(DynamicsProcessorEffect::RatioID, 4.);
  effect.setParameter(DynamicsProcessorEffect::ReleaseTimeID, Time::seconds(5.));
  effect.setParameter(DynamicsProcessorEffect::ThresholdID, Volume::dB(6.));
  effect.setParameter(DynamicsProcessorEffect::TypeID, DynamicsProcessorEffect::Type::Expander);
  auto tmp = effect.getParameters();
  
  REQUIRE_THAT(tmp.attackTime.seconds(), Catch::Matchers::WithinAbs(2., 0.0001));
  REQUIRE(tmp.hardStop == false);
  REQUIRE_THAT(tmp.outputGain.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.ratio, Catch::Matchers::WithinAbs(4., 0.0001));
  REQUIRE_THAT(tmp.releaseTime.seconds(), Catch::Matchers::WithinAbs(5., 0.0001));
  REQUIRE_THAT(tmp.threshold.dB(), Catch::Matchers::WithinAbs(6., 0.0001));
  REQUIRE(tmp.type == DynamicsProcessorEffect::Type::Expander);


  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);  
  
  tmp = dynamic_cast<DynamicsProcessorEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.attackTime.seconds(), Catch::Matchers::WithinAbs(2., 0.0001));
  REQUIRE(tmp.hardStop == false);
  REQUIRE_THAT(tmp.outputGain.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.ratio, Catch::Matchers::WithinAbs(4., 0.0001));
  REQUIRE_THAT(tmp.releaseTime.seconds(), Catch::Matchers::WithinAbs(5., 0.0001));
  REQUIRE_THAT(tmp.threshold.dB(), Catch::Matchers::WithinAbs(6., 0.0001));
  REQUIRE(tmp.type == DynamicsProcessorEffect::Type::Expander);
}

TEST_CASE("FilterEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;      
  FilterEffect::Parameters parameters;
  parameters.frequency = Frequency::Hz(1.);
  parameters.gain = Volume::dB(1.);
  parameters.q = 1.;
  parameters.type = FilterEffect::Type::AllPass;
  
  FilterEffect effect(parameters);
  effect.setParameter(FilterEffect::FrequencyID, Frequency::Hz(2.));
  effect.setParameter(FilterEffect::GainID, Volume::dB(3.));
  effect.setParameter(FilterEffect::Q_ID, 4.);
  effect.setParameter(FilterEffect::TypeID, FilterEffect::Type::BandPass);


  auto tmp = effect.getParameters();
  
  REQUIRE_THAT(tmp.frequency.Hz(), Catch::Matchers::WithinAbs(2., 0.0001));  
  REQUIRE_THAT(tmp.gain.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.q, Catch::Matchers::WithinAbs(4., 0.0001));  
  REQUIRE(tmp.type == FilterEffect::Type::BandPass);

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);  
  
  tmp = dynamic_cast<FilterEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.frequency.Hz(), Catch::Matchers::WithinAbs(2., 0.0001));  
  REQUIRE_THAT(tmp.gain.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.q, Catch::Matchers::WithinAbs(4., 0.0001));  
  REQUIRE(tmp.type == FilterEffect::Type::BandPass);
}

TEST_CASE("FlangerEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;      
  FlangerEffect::Parameters parameters;  
  parameters.feedback = Volume::dB(1);
  parameters.maxDelay = Time::seconds(1);
  parameters.minDelay = Time::seconds(1);
  parameters.rate = Frequency::Hz(1);
  
  FlangerEffect effect(parameters);  
  effect.setParameter(FlangerEffect::FeedbackID, Volume::dB(3.));
  effect.setParameter(FlangerEffect::MinDelayID, Time::seconds(4.));
  effect.setParameter(FlangerEffect::MaxDelayID, Time::seconds(5.));
  effect.setParameter(FlangerEffect::RateID, Frequency::Hz(6.));

  auto tmp = effect.getParameters();  
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.minDelay.seconds(), Catch::Matchers::WithinAbs(4., 0.0001));  
  REQUIRE_THAT(tmp.maxDelay.seconds(), Catch::Matchers::WithinAbs(5., 0.0001));    
  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(6., 0.0001));  

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);  
  
  tmp = dynamic_cast<FlangerEffect&>(*loaded.get()).getParameters();
  
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.minDelay.seconds(), Catch::Matchers::WithinAbs(4., 0.0001));  
  REQUIRE_THAT(tmp.maxDelay.seconds(), Catch::Matchers::WithinAbs(5., 0.0001));    
  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(6., 0.0001));  
}

TEST_CASE("LfoWah") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;      
  LfoWahEffect::Parameters parameters;
  parameters.minFrequency = Frequency::Hz(1);
  parameters.maxFrequency = Frequency::Hz(1);
  parameters.rate = Frequency::Hz(1);
  
  
  LfoWahEffect effect(parameters);  
  effect.setParameter(LfoWahEffect::MinFrequencyID, Frequency::Hz(2.));
  effect.setParameter(LfoWahEffect::MaxFrequencyID, Frequency::Hz(3.));
  effect.setParameter(LfoWahEffect::RateID, Frequency::Hz(4.));

  auto tmp = effect.getParameters();  
  REQUIRE_THAT(tmp.minFrequency.Hz(), Catch::Matchers::WithinAbs(2., 0.0001));  
  REQUIRE_THAT(tmp.maxFrequency.Hz(), Catch::Matchers::WithinAbs(3., 0.0001));  
  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(4., 0.0001));  

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  
  tmp = dynamic_cast<LfoWahEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.minFrequency.Hz(), Catch::Matchers::WithinAbs(2., 0.0001));
  REQUIRE_THAT(tmp.maxFrequency.Hz(), Catch::Matchers::WithinAbs(3., 0.0001));
  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(4., 0.0001));
}

TEST_CASE("PhaserEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  PhaserEffect::Parameters parameters;
  parameters.rate = Frequency::Hz(1.);
  parameters.depth = 1.;
  parameters.wet = Volume::dB(1.);
  parameters.dry = Volume::dB(1.);

  PhaserEffect effect(parameters);
  effect.setParameter(PhaserEffect::RateID, Frequency::Hz(2));
  effect.setParameter(PhaserEffect::DepthID, 3);
  effect.setParameter(PhaserEffect::WetID, Volume::dB(4));
  effect.setParameter(PhaserEffect::DryID, Volume::dB(5));

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));;
  REQUIRE_THAT(tmp.depth, Catch::Matchers::WithinAbs(3, 0.0001));;
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));;
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001));;

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<PhaserEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));;
  REQUIRE_THAT(tmp.depth, Catch::Matchers::WithinAbs(3, 0.0001));;
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));;
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001));;
}

TEST_CASE("PingPongDelayEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  PingPongDelayEffect::Parameters parameters;
  parameters.leftDelayTime = Time::seconds(1.);
  parameters.rightDelayTime = Time::seconds(1.);
  parameters.reservedDelayTime = Time::seconds(1.);
  parameters.dry = Volume::dB(1.);
  parameters.wet = Volume::dB(1.);
  parameters.feedback = Volume::dB(1.);

  PingPongDelayEffect effect(parameters);
  effect.setParameter(PingPongDelayEffect::LeftDelayTimeID, Time::seconds(2));
  effect.setParameter(PingPongDelayEffect::RightDelayTimeID, Time::seconds(3));
  effect.setParameter(PingPongDelayEffect::ReservedDelayTimeID, Time::seconds(4));
  effect.setParameter(PingPongDelayEffect::DryID, Volume::dB(5));
  effect.setParameter(PingPongDelayEffect::WetID, Volume::dB(6));
  effect.setParameter(PingPongDelayEffect::FeedbackID, Volume::dB(7));

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.leftDelayTime.seconds(), Catch::Matchers::WithinAbs(2, 0.0001));;
  REQUIRE_THAT(tmp.rightDelayTime.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));;
  REQUIRE_THAT(tmp.reservedDelayTime.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));;
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001));;
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(6, 0.0001));;
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(7, 0.0001));;

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<PingPongDelayEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.leftDelayTime.seconds(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.rightDelayTime.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.reservedDelayTime.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001));
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(6, 0.0001));
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(7, 0.0001));
}

TEST_CASE("PitchShiftEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  PitchShiftEffect::Parameters parameters;
  parameters.pitchShiftRatio = 1.;
  parameters.algorithm = PitchShiftEffect::Type::Normal;

  PitchShiftEffect effect(parameters);
  effect.setParameter(PitchShiftEffect::PitchShiftRatioID, 2);
  effect.setParameter(PitchShiftEffect::AlgorithmID, PitchShiftEffect::Type::PhaseLock);

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.pitchShiftRatio, Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE(tmp.algorithm == PitchShiftEffect::Type::PhaseLock);  

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<PitchShiftEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.pitchShiftRatio, Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE(tmp.algorithm == PitchShiftEffect::Type::PhaseLock);  
}

TEST_CASE("ReverbEffect") {
  //using namespace ZAudio;
  //using namespace ZAudio::Tools;
  //ReverbEffect::Parameters parameters;
  //parameters.rt60Time = Time::seconds(1.);
//
  //ReverbEffect effect(parameters);
  //effect.setParameter(ReverbEffect::Rt60TimeID, Time::seconds(2));
//
  //auto tmp = effect.getParameters();
  //REQUIRE_THAT(tmp.rt60Time.seconds(), Catch::Matchers::WithinAbs(2, 0.0001));
//
  //std::stringstream ss;
  //saveEffectToXML(effect, ss);
  //auto loaded = loadEffectFromXML(ss);
  //REQUIRE(loaded);
  //tmp = dynamic_cast<ReverbEffect&>(*loaded.get()).getParameters();
//
  //REQUIRE_THAT(tmp.rt60Time.seconds(), Catch::Matchers::WithinAbs(2, 0.0001));
}

TEST_CASE("ReverseDelayEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  ReverseDelayEffect::Parameters parameters;
  parameters.delayTime = Time::seconds(1.);
  parameters.dry = Volume::dB(1.);
  parameters.wet = Volume::dB(1.);
  parameters.feedback = Volume::dB(1.);

  ReverseDelayEffect effect(parameters);
  effect.setParameter(ReverseDelayEffect::DelayTimeID, Time::seconds(2));
  effect.setParameter(ReverseDelayEffect::DryID, Volume::dB(3));
  effect.setParameter(ReverseDelayEffect::WetID, Volume::dB(4));
  effect.setParameter(ReverseDelayEffect::FeedbackID, Volume::dB(5));

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.delayTime.seconds(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(5, 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<ReverseDelayEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.delayTime.seconds(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(5, 0.0001));
}

TEST_CASE("RobotEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  RobotEffect::Parameters parameters;
  parameters.hopRatio = 1.;

  RobotEffect effect(parameters);
  effect.setParameter(RobotEffect::HopRatioID, 2);

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.hopRatio, Catch::Matchers::WithinAbs(2, 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<RobotEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.hopRatio, Catch::Matchers::WithinAbs(2, 0.0001));
}

TEST_CASE("SequenceFilterEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  SequenceFilterEffect::Parameters parameters;
  parameters.changeFrequency = Frequency::Hz(1.);
  parameters.minFrequency = Frequency::Hz(1.);
  parameters.maxFrequency = Frequency::Hz(1.);
  parameters.filterQ = 1.;

  SequenceFilterEffect effect(parameters);
  effect.setParameter(SequenceFilterEffect::ChangeFrequencyID, Frequency::Hz(2));
  effect.setParameter(SequenceFilterEffect::MinFrequencyID, Frequency::Hz(3));
  effect.setParameter(SequenceFilterEffect::MaxFrequencyID, Frequency::Hz(4));
  effect.setParameter(SequenceFilterEffect::FilterQ_ID, 5);

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.changeFrequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minFrequency.Hz(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxFrequency.Hz(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.filterQ, Catch::Matchers::WithinAbs(5, 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<SequenceFilterEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.changeFrequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minFrequency.Hz(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxFrequency.Hz(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.filterQ, Catch::Matchers::WithinAbs(5, 0.0001));
}

TEST_CASE("StereoChorusEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  StereoChorusEffect::Parameters parameters;
  parameters.rate = Frequency::Hz(1.);
  parameters.minDelay = Time::seconds(1.);
  parameters.maxDelay = Time::seconds(1.);  

  StereoChorusEffect effect(parameters);
  effect.setParameter(StereoChorusEffect::RateID, Frequency::Hz(2));
  effect.setParameter(StereoChorusEffect::MinDelayID, Time::seconds(3));
  effect.setParameter(StereoChorusEffect::MaxDelayID, Time::seconds(4));  

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));  

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<StereoChorusEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));  
}

TEST_CASE("StereoFlangerEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  StereoFlangerEffect::Parameters parameters;
  parameters.rate = Frequency::Hz(1.);
  parameters.minDelay = Time::seconds(1.);
  parameters.maxDelay = Time::seconds(1.);
  parameters.feedback = Volume::dB(1.);  

  StereoFlangerEffect effect(parameters);
  effect.setParameter(StereoFlangerEffect::RateID, Frequency::Hz(2));
  effect.setParameter(StereoFlangerEffect::MinDelayID, Time::seconds(3));
  effect.setParameter(StereoFlangerEffect::MaxDelayID, Time::seconds(4));
  effect.setParameter(StereoFlangerEffect::FeedbackID, Volume::dB(5));  

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(5, 0.0001));  

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<StereoFlangerEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.feedback.dB(), Catch::Matchers::WithinAbs(5, 0.0001));  
}

TEST_CASE("StereoPhaserEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  StereoPhaserEffect::Parameters parameters;
  parameters.rate = Frequency::Hz(1.);
  parameters.depth = 1.;
  parameters.wet = Volume::dB(1.);
  parameters.dry = Volume::dB(1.);

  StereoPhaserEffect effect(parameters);
  effect.setParameter(StereoPhaserEffect::RateID, Frequency::Hz(2));
  effect.setParameter(StereoPhaserEffect::DepthID, 3);
  effect.setParameter(StereoPhaserEffect::WetID, Volume::dB(4));
  effect.setParameter(StereoPhaserEffect::DryID, Volume::dB(5));

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.depth, Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<StereoPhaserEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.depth, Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001));
}

TEST_CASE("TremoloEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  TremoloEffect::Parameters parameters;
  parameters.frequency = Frequency::Hz(1.);
  parameters.minGain = Volume::dB(1.);
  parameters.maxGain = Volume::dB(1.);

  TremoloEffect effect(parameters);
  effect.setParameter(TremoloEffect::FrequencyID, Frequency::Hz(2));
  effect.setParameter(TremoloEffect::MinGainID, Volume::dB(3));
  effect.setParameter(TremoloEffect::MaxGainID, Volume::dB(4));

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.frequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minGain.dB(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxGain.dB(), Catch::Matchers::WithinAbs(4, 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<TremoloEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.frequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minGain.dB(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxGain.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
}

TEST_CASE("VibratoEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  VibratoEffect::Parameters parameters;
  parameters.rate = Frequency::Hz(1.);
  parameters.minDelay = Time::seconds(1.);
  parameters.maxDelay = Time::seconds(1.);
  parameters.depth = 1.;

  VibratoEffect effect(parameters);
  effect.setParameter(VibratoEffect::RateID, Frequency::Hz(2));
  effect.setParameter(VibratoEffect::MinDelayID, Time::seconds(3));
  effect.setParameter(VibratoEffect::MaxDelayID, Time::seconds(4));
  effect.setParameter(VibratoEffect::DepthID, 0.5);

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.depth, Catch::Matchers::WithinAbs(0.5, 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<VibratoEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(tmp.depth, Catch::Matchers::WithinAbs(0.5, 0.0001));
}

TEST_CASE("VolumeControlEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  VolumeControlEffect::Parameters parameters;
  parameters.volumeChange = Volume::dB(1.);

  VolumeControlEffect effect(parameters);
  effect.setParameter(VolumeControlEffect::VolumeChangeID, Volume::dB(2));

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.volumeChange.dB(), Catch::Matchers::WithinAbs(2, 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<VolumeControlEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.volumeChange.dB(), Catch::Matchers::WithinAbs(2, 0.0001));
}

TEST_CASE("SerialEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  
  VibratoEffect::Parameters vibratoParameters;
  vibratoParameters.rate = Frequency::Hz(1.);
  vibratoParameters.minDelay = Time::seconds(1.);
  vibratoParameters.maxDelay = Time::seconds(1.);
  vibratoParameters.depth = 1.;

  TremoloEffect::Parameters tremoloParameters;
  tremoloParameters.frequency = Frequency::Hz(1.);
  tremoloParameters.minGain = Volume::dB(1.);
  tremoloParameters.maxGain = Volume::dB(1.);

  StereoPhaserEffect::Parameters stereoPhaserParameters;
  stereoPhaserParameters.rate = Frequency::Hz(1.);
  stereoPhaserParameters.depth = 1.;
  stereoPhaserParameters.wet = Volume::dB(1.);
  stereoPhaserParameters.dry = Volume::dB(1.);  

  SerialEffect serial(3);
  serial.setEffect(0, std::make_unique<VibratoEffect>(vibratoParameters));
  serial.setEffect(1, std::make_unique<TremoloEffect>(tremoloParameters));
  serial.setEffect(2, std::make_unique<StereoPhaserEffect>(stereoPhaserParameters));

  serial.setParameter(0, VibratoEffect::RateID, Frequency::Hz(2));
  serial.setParameter(0, VibratoEffect::MinDelayID, Time::seconds(3));
  serial.setParameter(0, VibratoEffect::MaxDelayID, Time::seconds(4));
  serial.setParameter(0, VibratoEffect::DepthID, 0.5);
  serial.setParameter(1, TremoloEffect::FrequencyID, Frequency::Hz(2));
  serial.setParameter(1, TremoloEffect::MinGainID, Volume::dB(3));
  serial.setParameter(1, TremoloEffect::MaxGainID, Volume::dB(4));  
  serial.setParameter(2, StereoPhaserEffect::RateID, Frequency::Hz(2));
  serial.setParameter(2, StereoPhaserEffect::DepthID, 3);
  serial.setParameter(2, StereoPhaserEffect::WetID, Volume::dB(4));
  serial.setParameter(2, StereoPhaserEffect::DryID, Volume::dB(5));

  const auto& vibrato = dynamic_cast<const VibratoEffect&>(serial.getEffect(0));
  const auto& tremolo = dynamic_cast<const TremoloEffect&>(serial.getEffect(1));
  const auto& stereoPhaser = dynamic_cast<const StereoPhaserEffect&>(serial.getEffect(2));
  REQUIRE_THAT(vibrato.getParameters().rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(vibrato.getParameters().minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(vibrato.getParameters().maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(vibrato.getParameters().depth, Catch::Matchers::WithinAbs(0.5, 0.0001));
  REQUIRE_THAT(tremolo.getParameters().frequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tremolo.getParameters().minGain.dB(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tremolo.getParameters().maxGain.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(stereoPhaser.getParameters().rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(stereoPhaser.getParameters().depth, Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(stereoPhaser.getParameters().wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(stereoPhaser.getParameters().dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001)); 

  std::stringstream ss;
  saveEffectToXML(serial, ss);  
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  
  const auto& serial2 = dynamic_cast<const SerialEffect&>(*loaded.get());
  const auto& vibrato2 = dynamic_cast<const VibratoEffect&>(serial2.getEffect(0));
  const auto& tremolo2 = dynamic_cast<const TremoloEffect&>(serial2.getEffect(1));
  const auto& stereoPhaser2 = dynamic_cast<const StereoPhaserEffect&>(serial2.getEffect(2));
  REQUIRE_THAT(vibrato2.getParameters().rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(vibrato2.getParameters().minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(vibrato2.getParameters().maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(vibrato2.getParameters().depth, Catch::Matchers::WithinAbs(0.5, 0.0001));
  REQUIRE_THAT(tremolo2.getParameters().frequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tremolo2.getParameters().minGain.dB(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tremolo2.getParameters().maxGain.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(stereoPhaser2.getParameters().rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(stereoPhaser2.getParameters().depth, Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(stereoPhaser2.getParameters().wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(stereoPhaser2.getParameters().dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001));
}

TEST_CASE("ParallelEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  
  VibratoEffect::Parameters vibratoParameters;
  vibratoParameters.rate = Frequency::Hz(1.);
  vibratoParameters.minDelay = Time::seconds(1.);
  vibratoParameters.maxDelay = Time::seconds(1.);
  vibratoParameters.depth = 1.;

  TremoloEffect::Parameters tremoloParameters;
  tremoloParameters.frequency = Frequency::Hz(1.);
  tremoloParameters.minGain = Volume::dB(1.);
  tremoloParameters.maxGain = Volume::dB(1.);

  StereoPhaserEffect::Parameters stereoPhaserParameters;
  stereoPhaserParameters.rate = Frequency::Hz(1.);
  stereoPhaserParameters.depth = 1.;
  stereoPhaserParameters.wet = Volume::dB(1.);
  stereoPhaserParameters.dry = Volume::dB(1.);  

  ParallelEffect parallel(FrameFormat::Stereo, FrameFormat::Stereo, 3);
  parallel.setEffect(0, std::make_unique<VibratoEffect>(vibratoParameters));
  parallel.setEffect(1, std::make_unique<TremoloEffect>(tremoloParameters));
  parallel.setEffect(2, std::make_unique<StereoPhaserEffect>(stereoPhaserParameters));

  parallel.setParameter(0, VibratoEffect::RateID, Frequency::Hz(2));
  parallel.setParameter(0, VibratoEffect::MinDelayID, Time::seconds(3));
  parallel.setParameter(0, VibratoEffect::MaxDelayID, Time::seconds(4));
  parallel.setParameter(0, VibratoEffect::DepthID, 0.5);
  parallel.setParameter(1, TremoloEffect::FrequencyID, Frequency::Hz(2));
  parallel.setParameter(1, TremoloEffect::MinGainID, Volume::dB(3));
  parallel.setParameter(1, TremoloEffect::MaxGainID, Volume::dB(4));  
  parallel.setParameter(2, StereoPhaserEffect::RateID, Frequency::Hz(2));
  parallel.setParameter(2, StereoPhaserEffect::DepthID, 3);
  parallel.setParameter(2, StereoPhaserEffect::WetID, Volume::dB(4));
  parallel.setParameter(2, StereoPhaserEffect::DryID, Volume::dB(5));

  const auto& vibrato = dynamic_cast<const VibratoEffect&>(parallel.getEffect(0));
  const auto& tremolo = dynamic_cast<const TremoloEffect&>(parallel.getEffect(1));
  const auto& stereoPhaser = dynamic_cast<const StereoPhaserEffect&>(parallel.getEffect(2));
  REQUIRE_THAT(vibrato.getParameters().rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(vibrato.getParameters().minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(vibrato.getParameters().maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(vibrato.getParameters().depth, Catch::Matchers::WithinAbs(0.5, 0.0001));
  REQUIRE_THAT(tremolo.getParameters().frequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tremolo.getParameters().minGain.dB(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tremolo.getParameters().maxGain.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(stereoPhaser.getParameters().rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(stereoPhaser.getParameters().depth, Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(stereoPhaser.getParameters().wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(stereoPhaser.getParameters().dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001)); 

  std::stringstream ss;
  saveEffectToXML(parallel, ss);  
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  
  const auto& parallel2 = dynamic_cast<const ParallelEffect&>(*loaded.get());
  const auto& vibrato2 = dynamic_cast<const VibratoEffect&>(parallel2.getEffect(0));
  const auto& tremolo2 = dynamic_cast<const TremoloEffect&>(parallel2.getEffect(1));
  const auto& stereoPhaser2 = dynamic_cast<const StereoPhaserEffect&>(parallel2.getEffect(2));
  REQUIRE_THAT(vibrato2.getParameters().rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(vibrato2.getParameters().minDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(vibrato2.getParameters().maxDelay.seconds(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(vibrato2.getParameters().depth, Catch::Matchers::WithinAbs(0.5, 0.0001));
  REQUIRE_THAT(tremolo2.getParameters().frequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tremolo2.getParameters().minGain.dB(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tremolo2.getParameters().maxGain.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(stereoPhaser2.getParameters().rate.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(stereoPhaser2.getParameters().depth, Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(stereoPhaser2.getParameters().wet.dB(), Catch::Matchers::WithinAbs(4, 0.0001));
  REQUIRE_THAT(stereoPhaser2.getParameters().dry.dB(), Catch::Matchers::WithinAbs(5, 0.0001));
}

TEST_CASE("Spatial2dEffect") {
  using namespace ZAudio;
  using namespace ZAudio::Tools;
  Spatial2dEffect::Parameters parameters;
  parameters.lowPassFrequency = Frequency::Hz(1);
  parameters.maxEarDelay = Time::seconds(1);
  parameters.maxFilteredPart = Volume::dB(-1);
  parameters.minEarGain = Volume::dB(-1);
  parameters.soundAngle = 1;

  Spatial2dEffect effect(parameters);
  effect.setParameter(Spatial2dEffect::LowPassFrequencyID, Frequency::Hz(2));
  effect.setParameter(Spatial2dEffect::MaxEarDelayID, Time::seconds(3));
  effect.setParameter(Spatial2dEffect::MaxFilteredPartID, Volume::dB(-4));
  effect.setParameter(Spatial2dEffect::MinEarGainID, Volume::dB(-5));
  effect.setParameter(Spatial2dEffect::SoundAngleID, 6);

  auto tmp = effect.getParameters();
  REQUIRE_THAT(tmp.lowPassFrequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.maxEarDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxFilteredPart.dB(), Catch::Matchers::WithinAbs(-4, 0.0001));
  REQUIRE_THAT(tmp.minEarGain.dB(), Catch::Matchers::WithinAbs(-5, 0.0001));
  REQUIRE_THAT(tmp.soundAngle, Catch::Matchers::WithinAbs(6, 0.0001));

  std::stringstream ss;
  saveEffectToXML(effect, ss);
  auto loaded = loadEffectFromXML(ss);
  REQUIRE(loaded);
  tmp = dynamic_cast<Spatial2dEffect&>(*loaded.get()).getParameters();

  REQUIRE_THAT(tmp.lowPassFrequency.Hz(), Catch::Matchers::WithinAbs(2, 0.0001));
  REQUIRE_THAT(tmp.maxEarDelay.seconds(), Catch::Matchers::WithinAbs(3, 0.0001));
  REQUIRE_THAT(tmp.maxFilteredPart.dB(), Catch::Matchers::WithinAbs(-4, 0.0001));
  REQUIRE_THAT(tmp.minEarGain.dB(), Catch::Matchers::WithinAbs(-5, 0.0001));
  REQUIRE_THAT(tmp.soundAngle, Catch::Matchers::WithinAbs(6, 0.0001));
}
#endif