#pragma once

#include <memory>

#include <ZAudio/Effect.h>
#include <ZAudio/CommonTypes.h>

namespace  ZAudio {


class MonoToStereoAdapter : public Effect {
public:
  MonoToStereoAdapter() = default;

  MonoToStereoAdapter(const Effect& effect);
  FrameFormat getOutputFormat() const override;
  FrameFormat getInputFormat() const override;
  void process(std::span<const sample_t> in, std::span<sample_t> out) override;
  void setParameter(size_t id, ParameterValue value) override;
  void setParameter(size_t id1, size_t id2, ParameterValue value) override;
  ParameterValue getOutputValue(size_t id) override;
  void setSampleRate(Frequency sampleRate) override;
  std::unique_ptr<Effect> clone() const override;
  Result save(Tools::TreeDatabaseWriter writer) const override;
  Result load(Tools::TreeDatabaseReader reader) override;
  std::string getID() const override;
  int64_t getVersion() const override;
  uint32_t getTailTime() const override;
private:
  std::unique_ptr<Effect> left = nullptr;
  std::unique_ptr<Effect> right = nullptr;
}; 


} // namespace  ZAudio