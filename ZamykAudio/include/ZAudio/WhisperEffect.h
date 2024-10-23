#pragma once

#include <random>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/CircularBuffer.h>
#include <ZAudio/Effect.h>
#include <ZAudio/PhaseVocoder.h>

namespace ZAudio {

class WhisperEffect : public Effect {
public:
  struct Parameters {
    size_t frameSizeTwoPow = 10; // frame size is 2^frameSizeTwoPow so 2^11=2048
    double hopRatio = 0.1;
    Parameters() = default;
    explicit Parameters(size_t frameSize_p, double hopRatio_p);
  };
  
  enum : uint32_t {
    FrameSizeTwoPowID,
    HopRatioID
  };

  explicit WhisperEffect(Parameters parameters);

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
private:
  Parameters parameters;
  Tools::PhaseVocoder phaseVocoder;  
  std::mt19937 gen;
  std::uniform_real_distribution<double> dist;
};


} // namespace ZAudio