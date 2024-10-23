#pragma once

#include <ZAudio/Effect.h>
#include <ZAudio/AudioDetector.h>
#include <ZAudio/AnalogFilter.h>
#include <ZAudio/Smoother.h>

namespace ZAudio {


class AutoWahEffect : public Effect {
public:  
struct Parameters {
  Volume lowEnvelope = Volume::dB(-27);
  Volume highEnvelope = Volume::dB(-5);
  Time decay = Time::miliseconds(200);
  Frequency minFrequency = Frequency::Hz(300);
  Frequency maxFrequency = Frequency::Hz(4000);
  double q = 2.;
  
  Parameters() = default;
  Parameters(Volume lowEnvelope_p, Volume highEnvelope_p, Time decay_p, Frequency minFrequency_p, Frequency maxFrequency_p, double q_p);
};
  static constexpr uint32_t NumOfParameters = 6;
  enum : uint32_t {
    LowEnvelopeID,
    HighEnvelopeID,
    DecayID,
    MinFrequencyID,
    MaxFrequencyID,
    Q_ID
  };  

  AutoWahEffect(Parameters parameters_p) : parameters(parameters_p) {}

  FrameFormat getInputFormat() const override {
    return FrameFormat::Mono;
  }

  FrameFormat getOutputFormat() const override {
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
  Tools::AudioDetector detector;  
  Tools::AnalogFilter bandPass;    

  void updateFilter();
};




} // namespace