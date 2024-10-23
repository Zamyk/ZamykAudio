#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/CircularBuffer.h>
#include <ZAudio/Effect.h>
#include <ZAudio/AudioDetector.h>

namespace ZAudio {

class DynamicsProcessorEffect : public Effect {
public:
enum struct Type {
  Compressor,
  Expander
};
struct Parameters {
  Volume outputGain = Volume::dB(0);
  Time attackTime = Time::miliseconds(50);
  Time releaseTime = Time::miliseconds(50);
  Volume threshold = Volume::dB(-3);
  double ratio = 1.;
  bool hardStop = false;
  Type type;

  Parameters() = default;
  Parameters(Volume outputGain_p, Time attackTime_p, Time releaseTime_p, Volume threshold_p, double ratio_p, bool hardStop_p, Type type_p);
};
  static constexpr uint32_t NumOfParameters = 7;
  enum : uint32_t {
    OutputGainID,
    AttackTimeID,
    ReleaseTimeID,
    ThresholdID,
    RatioID,
    HardStopID,
    TypeID    
  };

  DynamicsProcessorEffect(Parameters parameters_p);  

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
  Tools::AudioDetector detector;

  Volume calculateGainChange(Volume inputVolume);
};


} // name space