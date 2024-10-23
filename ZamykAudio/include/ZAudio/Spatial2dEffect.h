#pragma once

#include <ZAudio/Effect.h>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/SimpleDelay.h>
#include <ZAudio/AnalogFilter.h>

namespace ZAudio::Tools {

class Spatial2dEar {
public:
  Spatial2dEar() = default;
  Spatial2dEar(Frequency sampleRate_p, Time maxDelay_p, Volume minGain_p, Volume maxFilteredPart_p, Frequency lowPassFrequency_p, double angle_p);
  sample_t process(sample_t in);
  double calculateDistance(double angle);

  void setMaxDelay(Time maxDelay_p);
  void setMinGain(Volume minGain_p);
  void setMaxFilteredPart(Volume maxFilteredPart_p);  
  void setAngle(double angle);  
  void setLowPassFrequency(Frequency frequency);
private:  
  Frequency sampleRate;
  Time maxDelay;
  Volume minGain;
  Volume maxFilteredPart;
  double angle = 0.;
  Volume gain = Volume::linear(1.);
  SimpleDelay delay;
  AnalogFilter filter;
  Volume filterPart = Volume::linear(0.);  

  void update();
};


} // namespace ZAudio::Tools

namespace ZAudio {


class Spatial2dEffect : public Effect {
public:
struct Parameters {
  Time maxEarDelay = Time::miliseconds(0.6);
  Volume minEarGain = Volume::dB(-1.);  
  Volume maxFilteredPart = Volume::linear(0.3);
  Frequency lowPassFrequency = Frequency::Hz(300);
  double soundAngle = 0.;
};

  static constexpr size_t NumOfParameters = 5;
  enum : uint32_t {
    MaxEarDelayID,
    MinEarGainID,
    MaxFilteredPartID,
    LowPassFrequencyID,
    SoundAngleID
  };

  Spatial2dEffect(Parameters parameters_p);
  
  FrameFormat getOutputFormat() const override {
    return FrameFormat::Stereo;
  }

  FrameFormat getInputFormat() const override {
    return FrameFormat::Mono;
  }

  void process(std::span<const sample_t> in, std::span<sample_t> out) override;
  void setParameter(size_t id, ParameterValue value) override;  
  void setSampleRate(Frequency sampleRate_p) override;
  std::unique_ptr<Effect> clone() const override;
  Result save(Tools::TreeDatabaseWriter writer) const override;
  Result load(Tools::TreeDatabaseReader reader) override;
  std::string getID() const override;
  int64_t getVersion() const override;
  uint32_t getTailTime() const override;
  Parameters getParameters() const;
private:
  Parameters parameters;  
  Frequency sampleRate;
  Tools::Spatial2dEar left;
  Tools::Spatial2dEar right;      
};


} // namespace ZAudio