#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/CircularBuffer.h>
#include <ZAudio/Effect.h>
#include <ZAudio/PhaseVocoder.h>
#include <ZAudio/SampleRateConversion.h>

namespace ZAudio {

class PitchShiftEffect : public Effect {
public:    
  using Type = Tools::PhaseCorrector::Algorithm;
  struct Parameters {
    double pitchShiftRatio = 1.;
    Type algorithm = Type::Normal;

    Parameters() = default;
    Parameters(double pitchShiftRatio_p, Type algorithm_p);
  };
  
  PitchShiftEffect(Parameters parameters_p);

  static constexpr uint32_t NumOfParameters = 2;
  enum : uint32_t {
    PitchShiftRatioID,
    AlgorithmID
  };

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
  size_t frameSize = 4096;
  size_t inputHopSize = frameSize / 4;
  size_t outputHopSize = 0;

  Tools::PhaseVocoder phaseVocoder;      
  size_t outInd = 0;
  std::vector<sample_t> inputBuffer;
  std::vector<sample_t> helper;
  std::vector<sample_t> outputBuffer;    
};


} // namespace ZAudio