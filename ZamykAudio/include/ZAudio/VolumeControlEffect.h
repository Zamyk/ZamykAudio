#pragma once

#include <ZAudio/Effect.h>
#include <ZAudio/Smoother.h>
#include <ZAudio/CommonTypes.h>

namespace ZAudio {


class VolumeControlEffect : public Effect {
public:    
  struct Parameters {
    Volume volumeChange;
    Volume maxChangePerSecond;
    Parameters() = default;
    Parameters(Volume volumeChange_p, Volume maxChangePerSecond_p = Volume::dB(30)) : volumeChange(volumeChange_p), maxChangePerSecond(maxChangePerSecond_p) {}
  };  
  static constexpr uint32_t NumOfParameters = 2;
  enum : uint32_t {
    VolumeChangeID,
    MaxChangePerSecondID,
    SetVolumeChangeNoSmoothingID
  };

  explicit VolumeControlEffect(Parameters parameters);

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
  Parameters parameters;
  Parameters smoothedParameters;
  Tools::Smoother<Volume> volumeChangeSmoother;
};


} // namespace ZAudio

