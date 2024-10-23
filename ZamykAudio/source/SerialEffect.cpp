#include <ZAudio/SerialEffect.h>
#include <ZAudio/BypassEffect.h>

namespace ZAudio {
  

SerialEffect::SerialEffect(size_t n) : effects(n) {
  assert(effects.size());    
  for(auto& effect : effects) {
    effect = std::make_unique<BypassEffect>(FrameFormat::Mono, FrameFormat::Mono);
  }      
}

void SerialEffect::setEffect(size_t i, std::unique_ptr<Effect> effect) {    
  effects[i] = std::move(effect);
  if(sampleRateSet) {
    effects[i]->setSampleRate(sampleRate);
  }
}

FrameFormat SerialEffect::getOutputFormat() const {
  return effects.back()->getOutputFormat();
}
  
FrameFormat SerialEffect::getInputFormat() const {
  return effects.front()->getOutputFormat();
}

void SerialEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  std::array<sample_t, Tools::MaxNumberOfChannels> frame1;
  std::array<sample_t, Tools::MaxNumberOfChannels> frame2;
  std::fill(frame1.begin(), frame1.end(), 0.);
  std::fill(frame2.begin(), frame2.end(), 0.);
  std::fill(out.begin(), out.end(), 0.);

  for(size_t i = 0; i < Tools::numberOfChannels(getInputFormat()); i++) {
    frame1[i] = in[i];
  }

  for(auto it = effects.begin(); std::next(it) != effects.end(); ++it) {
    (*it)->process(frame1, frame2);
    Tools::convertFrames(frame2, (*it)->getOutputFormat(), frame1, (*std::next(it))->getInputFormat());
  }
  effects.back()->process(frame1, out);
}

void SerialEffect::setParameter(size_t id, ParameterValue value) {}

void SerialEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  sampleRateSet = true;
  for(auto& effect : effects) {
    effect->setSampleRate(sampleRate);
  }
}

void SerialEffect::setParameter(size_t effectID, size_t id, ParameterValue value) {
  effects[effectID]->setParameter(id, value);
}

uint32_t SerialEffect::getTailTime() const {
  uint32_t time = 0;
  for(auto& effect : effects) {
    time += effect->getTailTime();
  }
  return time;
}

std::unique_ptr<Effect> SerialEffect::clone() const {
  auto effect = std::make_unique<SerialEffect>(effects.size());
  for(size_t i = 0; i < effects.size(); i++) {
    effect->effects[i] = std::move(effects[i]->clone());
  }
  return std::move(effect);
}

Result SerialEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result res = writer.addValue("NumberOfEffects", effects.size());
  for(size_t i = 0; i < effects.size(); i++) {
    auto child = writer.addChild("effect" + std::to_string(i));
    if(!child) {
      res &= Result::error("cannot create child: effect" + std::to_string(i));
      return res;
    }
    res &= Tools::EffectSerializer::instance().save(*child, *effects[i]);
  }
  return res;
}

Result SerialEffect::load(Tools::TreeDatabaseReader reader) {
  size_t n = 0;
  Result res = reader.getValue("NumberOfEffects", n);
  effects.resize(n);
  for(size_t i = 0; i < effects.size(); i++) {
    auto child = reader.getChild("effect" + std::to_string(i));
    if(!child) {
      res &= Result::error("no child: effect" + std::to_string(i));
      return res;
    }
    auto tmp = Tools::EffectSerializer::instance().load(*child);
    if(!tmp) {
      return res & Result::error(tmp.getDescription());
    }
    effects[i] = std::move(tmp.get());
  }
  return Result::success();
}

std::string SerialEffect::getID() const {
  return "ZA_SerialEffect";
}

int64_t SerialEffect::getVersion() const {
  return 1;
}

const Effect& SerialEffect::getEffect(size_t i) const {
  return *effects[i];
}


} // namespace ZAudio