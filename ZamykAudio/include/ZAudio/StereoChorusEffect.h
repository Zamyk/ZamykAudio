#pragma once

#include <ZAudio/Effect.h>
#include <ZAudio/ModulatedDelay.h>

namespace ZAudio {


class StereoChorusEffect : public Effect {
public:
struct Parameters {
  Frequency rate = Frequency::Hz(2);
  Time minDelay = Time::miliseconds(3.);
  Time maxDelay = Time::miliseconds(7.);    

  Parameters() = default;
  Parameters(Frequency rate_p, Time minDelay_p, Time maxDelay_p);
};
  static constexpr uint32_t NumOfParameters = 3;
  enum : uint32_t {
    RateID,
    MinDelayID,
    MaxDelayID    
  };

  explicit StereoChorusEffect(Parameters parameters_p);

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
  Tools::ModulatedDelay middleModDelay;
  Tools::ModulatedDelay rightModDelay;

  Frequency getLeftDelayRate() const;
  Frequency getMiddleDelayRate() const;
  Frequency getRightDelayRate() const;  
};




} // namespace ZAudio