#include <ZAudio/BitCrusherEffect.h>

namespace {  


double calculateQuantizerMul(double bitDepth) {
  return (pow(2.0, bitDepth)) / 2.;
}


} // anonymous

namespace ZAudio {


BitCrusherEffect::BitCrusherEffect(Parameters parameters_p) : parameters(parameters_p), quantizerMul(calculateQuantizerMul(parameters_p.bitDepth)) {}  

void BitCrusherEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  sample_t y = (static_cast<int32_t>(in[0] * quantizerMul)) / quantizerMul;
  out[0] = y * parameters.wet.linear() + in[0] * parameters.dry.linear(); 
}

void BitCrusherEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case BitDepthID:
      parameters.bitDepth = value.getNonInteger();
      quantizerMul = calculateQuantizerMul(parameters.bitDepth);
    break;

    case WetID:
      parameters.wet = value.getVolume();
    break;

    case DryID:
      parameters.dry = value.getVolume();
    break;

    default:
      assert(false);
  }
}

uint32_t BitCrusherEffect::getTailTime() const {
  return 0;
}

void BitCrusherEffect::setSampleRate(Frequency sampleRate) {
  quantizerMul = calculateQuantizerMul(parameters.bitDepth);
}


std::unique_ptr<Effect> BitCrusherEffect::clone() const {
  return std::make_unique<BitCrusherEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, BitCrusherEffect::NumOfParameters> names = {
    "BitDepth",
    "Wet",
    "Dry"
  };
  return names[ind];
}

Result BitCrusherEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result result = Result::success();
  result &= writer.addValue(getName(BitDepthID), parameters.bitDepth);
  result &= writer.addValue(getName(WetID), parameters.wet);
  result &= writer.addValue(getName(DryID), parameters.dry);
  return result;
}

Result BitCrusherEffect::load(Tools::TreeDatabaseReader reader) {
  Result result = Result::success();
  result &= reader.getValue(getName(BitDepthID), parameters.bitDepth);
  result &= reader.getValue(getName(WetID), parameters.wet);
  result &= reader.getValue(getName(DryID), parameters.dry);  
  return result;
}

std::string BitCrusherEffect::getID() const {
  return "ZA_BitCrusherEffect";
}

int64_t BitCrusherEffect::getVersion() const {
  return 1;
}

BitCrusherEffect::Parameters BitCrusherEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio



