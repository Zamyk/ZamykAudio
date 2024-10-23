#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/CircularBuffer.h>
#include <ZAudio/Effect.h>
#include <ZAudio/AudioDelay.h>

namespace ZAudio {

class PingPongDelayEffect : public Effect {
public:
struct Parameters {
  Time leftDelayTime;
  Time rightDelayTime;
  Time reservedDelayTime = Time::seconds(4);
  Volume dry;
  Volume wet;
  Volume feedback;

  Parameters() = default;
  Parameters(Time leftDelayTime_p, Time rightDelayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p);
  Parameters(Time leftDelayTime_p, Time rightDelayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p, Time reservedDelayTime_p);
};
  static constexpr uint32_t NumOfParameters = 6;
  enum : uint32_t {
    LeftDelayTimeID,
    RightDelayTimeID,
    ReservedDelayTimeID,
    DryID,
    WetID,
    FeedbackID
  };

  PingPongDelayEffect(Parameters parameters_p);  

  FrameFormat getOutputFormat() const override {
    return FrameFormat::Stereo;
  }

  FrameFormat getInputFormat() const override {
    return FrameFormat::Stereo;
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
  Parameters parameters;  
  Frequency sampleRate;
  Tools::AudioDelay leftDelay;
  Tools::AudioDelay rightDelay;
};


} // namespace ZAudio