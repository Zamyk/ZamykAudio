#pragma once

#include <random>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/Effect.h>
#include <ZAudio/AnalogFilter.h>

namespace ZAudio {


class SequenceFilterEffect : public Effect {
public:
struct Parameters {
  Frequency changeFrequency = Frequency::Hz(7);
  Frequency minFrequency = Frequency::Hz(100);  
  Frequency maxFrequency = Frequency::Hz(1000);  
  double filterQ = 5.;  

  Parameters() = default;
  Parameters(Frequency changeFrequency_p, Frequency minFrequency_p, Frequency maxFrequency_p, double filterQ_p);
};
  static constexpr uint32_t NumOfParameters = 4;
  enum : uint32_t {
    ChangeFrequencyID,
    MinFrequencyID,
    MaxFrequencyID,
    FilterQ_ID
  };
  SequenceFilterEffect(Parameters parameters_p);

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
  Tools::AnalogFilter filter;     
  uint64_t periodInSamples = 0.;
  uint64_t counter = 0;    
  std::mt19937 gen;
  std::uniform_real_distribution<double> dist;

  void updateFilter();
};

} // namespace ZAudio