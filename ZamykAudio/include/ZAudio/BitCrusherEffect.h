#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/Effect.h>

namespace ZAudio {


class BitCrusherEffect : public Effect {
public:  
struct Parameters {
  double bitDepth = 16.;
  Volume wet = Volume::linear(1);
  Volume dry = Volume::linear(0);
};

  static constexpr uint32_t NumOfParameters = 3;
  enum : uint32_t {
    BitDepthID,
    WetID,
    DryID
  };  

  BitCrusherEffect(Parameters parameters);

  FrameFormat getOutputFormat() const override {
    return FrameFormat::Mono;
  }

  FrameFormat getInputFormat() const override {
    return FrameFormat::Mono;
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
  double quantizerMul = 1.;
};


} // namespace