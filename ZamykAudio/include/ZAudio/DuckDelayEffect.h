#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/Effect.h>
#include <ZAudio/AudioDelay.h>
#include <ZAudio/AudioDetector.h>

namespace ZAudio::Tools {


class DelayGainController {
public:
  DelayGainController(Frequency sampleRate_p, Volume threshold_p, Time recoveryTime_p, Volume wetMin_p, Volume wetMax_p);
  DelayGainController() = default;
  Volume process(Volume env);

  void setThreshold(Volume threshold_p);
  void setRecoveryTime(Time time_p);
  void setWetMin(Volume wetMin_p);
  void setWetMax(Volume wetMax_p);  

private:
  Frequency sampleRate;
  Volume threshold;
  Time recoveryTime;  
  Volume wetMin;  
  Volume wetMax;
  Volume current;
  double increaseDB = 0.;

  double calculateIncrese() const;
};


} // namespace ZAudio::Tools


namespace ZAudio {


class DuckDelayEffect : public Effect {
public:
struct Parameters {
  Time delayTime;
  Time reservedDelayTime = Time::seconds(2);
  Volume dry = Volume::dB(0);
  Volume feedback = Volume::dB(-3);
  Volume wetMin = Volume::dB(-12);
  Volume wetMax = Volume::dB(0);  
  Volume threshold = Volume::dB(-6);
  Time recoveryTime = Time::seconds(0.2);
  Time attackTime = Time::miliseconds(50);
  Time releaseTime = Time::miliseconds(50);  

  Parameters() = default;
  Parameters(Time delayTime_p);
  Parameters(Time delayTime_p, Volume dry, Volume feedback);
  Parameters(Time delayTime_p, Volume dry_p, Volume feedback_p, Volume wetMin_p, Volume wetMax_p, Volume threshold_p, Time recoveryTime_p);
};
  static constexpr uint32_t NumOfParameters = 10;
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

  DuckDelayEffect(Parameters parameters_p) : parameters(parameters_p) {}
  ~DuckDelayEffect() {}

  FrameFormat getOutputFormat() const override {
    return FrameFormat::Mono;
  }

  FrameFormat getInputFormat() const override {
    return FrameFormat::Mono;
  }

  void process(std::span<const sample_t> in, std::span<sample_t> out) override;
  void setParameter(size_t id, ParameterValue value) override;
  void setSampleRate(Frequency sampleRate_p) override;
  uint32_t getTailTime() const override;

  std::unique_ptr<Effect> clone() const override;
  Result save(Tools::TreeDatabaseWriter writer) const override;
  Result load(Tools::TreeDatabaseReader reader) override;
  std::string getID() const override;
  int64_t getVersion() const override;
  Parameters getParameters() const;
private:
  Frequency sampleRate;  
  Parameters parameters;  
  Tools::AudioDelay delay;  
  Tools::AudioDetector detector;
  Tools::DelayGainController gainControl;  
};


} // namespace ZAudio