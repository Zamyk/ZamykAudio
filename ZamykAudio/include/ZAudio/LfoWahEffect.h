#pragma once

#include <ZAudio/Effect.h>
#include <ZAudio/LowFrequencyOscillator.h>
#include <ZAudio/AudioDetector.h>
#include <ZAudio/AnalogFilter.h>

namespace ZAudio {


class LfoWahEffect : public Effect {
public:
struct Parameters {
  Frequency rate = Frequency::Hz(2);
  Frequency minFrequency = Frequency::Hz(350);
  Frequency maxFrequency = Frequency::Hz(2000);

  Parameters() = default;
  Parameters(Frequency rate_p, Frequency minFrequency_p, Frequency maxFrequency_p);
};
  static constexpr uint32_t NumOfParameters = 3;
  enum : uint32_t {
    RateID,
    MinFrequencyID,
    MaxFrequencyID
  };

  LfoWahEffect(Parameters parameters_p);

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
  Frequency currFrequency;
  Tools::LowFrequencyOscillator lfo;
  Tools::AnalogFilter bandPass;  

  void updateFilter();
};




}
