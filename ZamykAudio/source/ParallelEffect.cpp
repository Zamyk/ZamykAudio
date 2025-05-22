#include <ZAudio/ParallelEffect.h>
#include <ZAudio/BypassEffect.h>

namespace ZAudio {


ParallelEffect::ParallelEffect(FrameFormat inputFormat_p, FrameFormat outputFormat_p, size_t n)  :   
  effects(n),
  inputFormat(inputFormat_p),  
  outputFormat(outputFormat_p)
  {
    assert(effects.size());
    effects.front() = std::make_unique<BypassEffect>(inputFormat_p, outputFormat_p);
    for(size_t i = 1; i < effects.size(); i++) {
      effects[i] = std::make_unique<BypassEffect>(outputFormat_p, outputFormat_p);
    }      
  }

FrameFormat ParallelEffect::getOutputFormat() const {
  return inputFormat;
}
  
FrameFormat ParallelEffect::getInputFormat() const {
  return outputFormat;
}

void ParallelEffect::setEffect(size_t i, std::unique_ptr<Effect> effect) {
  effects[i] = std::move(effect);
  if(sampleRateSet) {
    effects[i]->setSampleRate(sampleRate);
  }
}

void ParallelEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  std::array<sample_t, Tools::MaxNumberOfChannels> frame1;
  std::array<sample_t, Tools::MaxNumberOfChannels> frame2;
  std::fill(frame1.begin(), frame1.end(), 0.);
  std::fill(frame2.begin(), frame2.end(), 0.);

  std::fill(out.begin(), out.end(), 0.);

  for(auto& effect : effects) {
    Tools::convertFrames(in, inputFormat, frame1, effect->getInputFormat());
    effect->process(frame1, frame2);
    Tools::convertFrames(frame2, effect->getOutputFormat(), frame1, outputFormat);

    for(size_t i = 0; i < Tools::numberOfChannels(outputFormat); i++) {
      out[i] += frame1[i] / static_cast<double>(effects.size());
    }
  }
}

void ParallelEffect::setParameter(size_t id, ParameterValue value) {}

void ParallelEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  sampleRateSet = true;
  for(auto& effect : effects) {
    effect->setSampleRate(sampleRate);
  }
}

void ParallelEffect::setParameter(size_t effectID, size_t id, ParameterValue value) {
  effects[effectID]->setParameter(id, value);
}

uint32_t ParallelEffect::getTailTime() const {
  uint32_t maxTime = 0;
  for(auto& effect : effects) {
    maxTime = std::max(maxTime, effect->getTailTime());
  }
  return maxTime;
}

std::unique_ptr<Effect> ParallelEffect::clone() const {
  auto effect = std::make_unique<ParallelEffect>(inputFormat, outputFormat, effects.size());
  for(size_t i = 0; i < effects.size(); i++) {
    effect->effects[i] = effects[i]->clone();
  }
  return std::move(effect);
}

Result ParallelEffect::save(Tools::TreeDatabaseWriter writer) const {
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

Result ParallelEffect::load(Tools::TreeDatabaseReader reader) {
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

std::string ParallelEffect::getID() const {
  return "ZA_ParallelEffect";
}

int64_t ParallelEffect::getVersion() const {
  return 1;
}

const Effect& ParallelEffect::getEffect(size_t i) const {
  return *effects[i];
}



} // namespace ZAudio