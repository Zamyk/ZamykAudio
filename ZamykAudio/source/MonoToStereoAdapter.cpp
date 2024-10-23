#include <ZAudio/MonoToStereoAdapter.h>

namespace ZAudio {


MonoToStereoAdapter::MonoToStereoAdapter(const Effect& effect) {
  left = effect.clone();
  right = effect.clone();
}
  
FrameFormat MonoToStereoAdapter::getOutputFormat() const {
  return FrameFormat::Stereo;
}

FrameFormat MonoToStereoAdapter::getInputFormat() const {
  return FrameFormat::Stereo;
}

void MonoToStereoAdapter::process(std::span<const sample_t> in, std::span<sample_t> out) {
  left->process(in, out);
  right->process(in.subspan(1, 1), out.subspan(1, 1));
}

void MonoToStereoAdapter::setParameter(size_t id, ParameterValue value) {
  left->setParameter(id, value);
  right->setParameter(id, value);
}

void MonoToStereoAdapter::setParameter(size_t id1, size_t id2, ParameterValue value) {
  left->setParameter(id1, id2, value);
}

ParameterValue MonoToStereoAdapter::getOutputValue(size_t id) {
  return left->getOutputValue(id); 
}  

void MonoToStereoAdapter::setSampleRate(Frequency sampleRate) {
  left->setSampleRate(sampleRate);
  right->setSampleRate(sampleRate);
}

std::unique_ptr<Effect> MonoToStereoAdapter::clone() const {
  if(left == nullptr) {
    return std::make_unique<MonoToStereoAdapter>();
  }
  return std::make_unique<MonoToStereoAdapter>(*left);
}

Result MonoToStereoAdapter::save(Tools::TreeDatabaseWriter writer) const {
  return Tools::EffectSerializer::instance().save(writer, *left);        
}

Result MonoToStereoAdapter::load(Tools::TreeDatabaseReader reader) {
  auto effect = Tools::EffectSerializer::instance().load(reader);
  if(!effect) {
    return Result::error(effect.getDescription());
  }
  left = std::move(effect.get());
  right = left->clone();
  return Result::success();
}

std::string MonoToStereoAdapter::getID() const {
  return "ZA_MonoToStereoAdapter";
}

int64_t MonoToStereoAdapter::getVersion() const {
  return 1;
}

uint32_t MonoToStereoAdapter::getTailTime() const {
  return left->getTailTime();
}


} // namespace ZAudio