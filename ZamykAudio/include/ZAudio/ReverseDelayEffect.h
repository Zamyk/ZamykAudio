#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/CircularBuffer.h>
#include <ZAudio/Effect.h>
#include <ZAudio/AudioDelay.h>

namespace ZAudio {

class ReverseDelayEffect : public Effect {
public:
struct Parameters {
  Time delayTime;
  Volume dry;
  Volume wet;
  Volume feedback;

  Parameters() = default;
  Parameters(Time delayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p);
};
  static constexpr uint32_t NumOfParameters = 4;
  enum : uint32_t {
    DelayTimeID,
    DryID,
    WetID,
    FeedbackID
  };

  ReverseDelayEffect(Parameters parameters_p);

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
  Tools::CircularBuffer<sample_t> buffer;
  Tools::CircularBuffer<sample_t>::Iterator curr;
};


} // namespace ZAudio