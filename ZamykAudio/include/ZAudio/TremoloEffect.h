#pragma once

#include <ZAudio/LowFrequencyOscillator.h>
#include <ZAudio/Effect.h>

namespace ZAudio {


class TremoloEffect : public Effect {
public:
struct Parameters {
  Frequency frequency = Frequency::Hz(1);
  Volume minGain;
  Volume maxGain;  

  Parameters() = default;
  Parameters(Frequency frequency_p, Volume minGain_p, Volume maxGain_p);
};
  static constexpr uint32_t NumOfParameters = 3;
  enum : uint32_t {
    FrequencyID,
    MinGainID,
    MaxGainID
  };

  explicit TremoloEffect(Parameters parameters_p);

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
  Tools::LowFrequencyOscillator lfo;
};


} // namespace ZAudio