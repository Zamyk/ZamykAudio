#include <ZAudio/BypassEffect.h>


namespace ZAudio {


BypassEffect::BypassEffect(FrameFormat inputFormat_p, FrameFormat outputFormat_p) : inputFormat(inputFormat_p), outputFormat(outputFormat_p) {}

void BypassEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  Tools::convertFrames(in, inputFormat, out, outputFormat);
}

uint32_t BypassEffect::getTailTime() const {
  return 0;
}

std::unique_ptr<Effect> BypassEffect::clone() const {
  return std::make_unique<BypassEffect>(inputFormat, outputFormat);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, 2> names = {
    "InputFormat",
    "OutputFormat"    
  };
  return names[ind];
}

Result BypassEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addEnumValue<FrameFormat>(getName(0), inputFormat, frameFormatToString);
  result &= writer.addEnumValue<FrameFormat>(getName(1), outputFormat, frameFormatToString);  
  return result;
}

Result BypassEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();
  result &= reader.getEnumValue<FrameFormat>(getName(0), inputFormat, frameFormatToString);
  result &= reader.getEnumValue<FrameFormat>(getName(1), outputFormat, frameFormatToString);
  return result;
}

std::string BypassEffect::getID() const {
  return "ZA_BypassEffect";
}

int64_t BypassEffect::getVersion() const {
  return 1;
}


} // namespace ZAudio