#pragma once

#include <ZAudio/Effect.h>
#include <ZAudio/PhaseShifter.h>

namespace ZAudio {


class StereoPhaserEffect : public Effect {
public:
struct Parameters {
  Frequency rate = Frequency::Hz(0.4);
  double depth = 1.;
  Volume wet = Volume::dB(-3);
  Volume dry = Volume::dB(-3);

  Parameters() = default;
  Parameters(Frequency rate_p, double depth_p, Volume wet_p, Volume dry_p);
};
  static constexpr uint32_t NumOfParameters = 4;
  enum : uint32_t {
    RateID,
    DepthID,
    WetID,
    DryID
  };

  explicit StereoPhaserEffect(Parameters parameters_p);

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
  Tools::PhaseShifter leftPhaseShifter;
  Tools::PhaseShifter rightPhaseShifter;
};




}