#pragma once

#include <vector>
#include <ZAudio/Effect.h>

namespace ZAudio {


class SerialEffect : public Effect {
public:

  SerialEffect(size_t n);
  void setEffect(size_t i, std::unique_ptr<Effect> effect);
  FrameFormat getOutputFormat() const override;
  FrameFormat getInputFormat() const override;
  void process(std::span<const sample_t> in, std::span<sample_t> out) override;
  void setParameter(size_t id, ParameterValue value) override;
  void setSampleRate(Frequency sampleRate_p) override;
  void setParameter(size_t effectID, size_t id, ParameterValue value) override;
  uint32_t getTailTime() const override;
  std::unique_ptr<Effect> clone() const override;
  Result save(Tools::TreeDatabaseWriter writer) const override;
  Result load(Tools::TreeDatabaseReader reader) override;
  std::string getID() const override;
  int64_t getVersion() const override;
  const Effect& getEffect(size_t i) const;

private:  
  bool sampleRateSet = false;
  Frequency sampleRate;
  std::vector<std::unique_ptr<Effect>> effects;
};


} // namespace ZAudio