#pragma once

#include <ZAudio/Effect.h>

namespace ZAudio {

class BypassEffect : public Effect {
public:  
  BypassEffect(FrameFormat inputFormat_p, FrameFormat outputFormat_p);

  FrameFormat getOutputFormat() const override {
    return outputFormat;  
  }

  FrameFormat getInputFormat() const override {
    return inputFormat;
  }

  void process(std::span<const sample_t> in, std::span<sample_t> out)override ;
  void setParameter(size_t id, ParameterValue value) override {}
  void setSampleRate(Frequency sampleRate) override {}
  uint32_t getTailTime() const override;

  std::unique_ptr<Effect> clone() const override;
  Result save(Tools::TreeDatabaseWriter writer) const override;
  Result load(Tools::TreeDatabaseReader reader) override;
  std::string getID() const override;
  int64_t getVersion() const override;
private:
  FrameFormat inputFormat;
  FrameFormat outputFormat;
};


} // namespace ZAudio