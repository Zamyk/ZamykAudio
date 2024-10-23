#pragma once

#include <ZAudio/Effect.h>
#include <ZAudio/ModulatedDelay.h>

namespace ZAudio {


class StereoFlangerEffect : public Effect {
public:
struct Parameters {
  Frequency rate = Frequency::Hz(2);
  Time minDelay = Time::miliseconds(0.);
  Time maxDelay = Time::miliseconds(7.);
  Volume feedback = Volume::dB(-3);  

  Parameters() = default;
  Parameters(Frequency rate_p, Time minDelay_p, Time maxDelay_p, Volume feedback_p);
};
  static constexpr uint32_t NumOfParameters = 4;
  enum : uint32_t {
    RateID,
    MinDelayID,
    MaxDelayID,
    FeedbackID
  };

  explicit StereoFlangerEffect(Parameters parameters_p);

  FrameFormat getOutputFormat() const override {
    return FrameFormat::Stereo;
  }

  FrameFormat getInputFormat() const override {
    return FrameFormat::Stereo;
  }

  void process(std::span<const sample_t> in, std::span<sample_t> out) override;
  void setParameter(size_t id, ParameterValue value) override;
  void setSampleRate(Frequency sampleRate) override;
  uint32_t getTailTime() const override;

  std::unique_ptr<Effect> clone() const override;
  Result save(Tools::TreeDatabaseWriter writer) const override;
  Result load(Tools::TreeDatabaseReader reader) override;
  std::string getID() const override;
  int64_t getVersion() const override;
  Parameters getParameters() const;
private:
  Parameters parameters;
  Tools::ModulatedDelay leftModDelay;
  Tools::ModulatedDelay rightModDelay;
};




}