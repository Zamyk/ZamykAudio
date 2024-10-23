#pragma once

#include <cmath>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/Effect.h>
#include <ZAudio/AnalogFilter.h>

namespace ZAudio {


class FilterEffect : public Effect {
public:
using Type = Tools::AnalogFilter::Type;
struct Parameters {      
  Type type = Type::ByPass;
  Frequency frequency;
  double q = 0.;
  Volume gain;

  Parameters() = default;
  Parameters(Type type_p, Frequency frequency_p, double q_p = 0., Volume gain_p = Volume::dB(0));
};
  static constexpr uint32_t NumOfParameters = 4;
  enum : uint32_t {
    TypeID,
    FrequencyID,
    Q_ID,
    GainID
  };
    
  FilterEffect(Parameters parameters_p);
  ~FilterEffect() {}  
   

  FrameFormat getOutputFormat() const override {
    return FrameFormat::Mono;
  }

  FrameFormat getInputFormat() const override {
    return FrameFormat::Mono;
  }

  void process(std::span<const sample_t> in, std::span<sample_t> out) override;
  void setParameter(size_t id, ParameterValue value) override;
  void updateFilter();
  void setSampleRate(Frequency sampleRate_p) override;
  uint32_t getTailTime() const override;

  std::unique_ptr<Effect> clone() const override;
  Result save(Tools::TreeDatabaseWriter writer) const override;
  Result load(Tools::TreeDatabaseReader reader) override;
  std::string getID() const override;
  int64_t getVersion() const override;
  Parameters getParameters() const;
private:
  Tools::AnalogFilter filter;
  Parameters parameters;
  Frequency sampleRate;  
};


} // namespace
