#include <numbers>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/Math.h>


// functions based on Will Pirkle audio-effects page:549
namespace ZAudio::Tools::WaveShapers {

enum struct WaveShaperType {
  Arraya, Sigmoid, Sigmoid2, HyperbolicTangent, ArcTangent, FuzzExponential1, FuzzExponential2, Exponential2, ArctangentSquareRoot,
  SquareSign, Cube, HardClipper, HalfWaveRectifier, FullWaveRectifier, SquareLaw, AbsoluteSquareRoot, 
};

sample_t arraya(sample_t x) {
  //(3x / 2)(1-x*x/3)
  return 3. * x * (1. - x * x / 3.) / 2.;
}

sample_t sigmoid(sample_t x, sample_t k) {
  // 2(1 / (1+e^-kx))-1
  return 2. * (1. / (1. + std::exp(-k * x))) - 1.;
}

sample_t sigmoid2(sample_t x) {
  // (e^x-1)(e+1)/(e^x+1)(e-1)
  auto ex = std::exp(x);
  auto e = std::numbers::e;

  return (ex - 1.) * (e + 1.) / (ex + 1.) * (e - 1.);
}

sample_t hyperbolicTangent(sample_t x, sample_t k) {
  // tanh(kx) / tanh(k)
  return tanh(k * x) / tanh(k);
}

sample_t arcTangent(sample_t x, sample_t k) {
  // atan(kx) / atan(k)
  return atan(k * x) / atan(k);
}

sample_t fuzzExponential1(sample_t x, sample_t k) {
  // sgn(x)(1-e^-|kx|)/(1-e^-k)
  return Math::sgn(x) * (1. - std::exp(-std::fabs(k * x))) / (1. - std::exp(-k));
}

sample_t fuzzExponential2(sample_t x) {
  // sgn(-x)(1-e^|x|)/(e-1)
  return Math::sgn(-x) * (1. - std::exp(std::fabs(x))) / (std::numbers::e - 1.);
}

sample_t exponential2(sample_t x) {
  // (e - e^(1-x))/(e-1)
  return (std::numbers::e - std::exp(1. - x)) / (std::numbers::e - 1.);
}

sample_t arctangetSquareRoot(sample_t x) {
  // 2.5tan^-1(0.9x) + 2.5sqrt(1 - (0.9x)^2 ) - 2.5
  return 2.5 * (1. / tan(0.9 * x)) + 2.5 *  sqrt(1 - (0.9 * x) * (0.9 * x)) - 2.5; 
}

sample_t squareSign(sample_t x) {
  // x^2*sgn(x)
  return x * x * Math::sgn(x);
}

sample_t cube(sample_t x) {
  // x^3
  return x * x * x;
}

sample_t hardClipper(sample_t x) {
  // |x| > 0.5 : 0.5sgn(x) 
  // otherwise : x
  return std::fabs(x) > 0.5 ? 0.5 * Math::sgn(x) : x;  
}

sample_t halfWaveRectifier(sample_t x) {
  // 0.5(x + |x|)
  return 0.5 * (x + std::fabs(x));
}

sample_t fullWaveRectifier(sample_t x) {
  // |x|
  return std::fabs(x);
}

sample_t squareLaw(sample_t x) {
  // x^2
  return x * x;
}

sample_t absoluteSquareRoot(sample_t x) {
  // sqrt(|x|)
  return sqrt(std::fabs(x));
}

sample_t process(WaveShaperType type, sample_t x, sample_t k) {
  switch (type) {
    case (WaveShaperType::Arraya):
      return arraya(x);
    break;
    case(WaveShaperType::Sigmoid):
      return sigmoid(x, k);
    break;
    case(WaveShaperType::Sigmoid2):
      return sigmoid2(x);
    break;
    case(WaveShaperType::HyperbolicTangent):    
      return hyperbolicTangent(x, k);
    break;
    case(WaveShaperType::ArcTangent):
      return arcTangent(x, k);
    break;
    case(WaveShaperType::FuzzExponential1):    
      return fuzzExponential1(x, k);
    break;
    case(WaveShaperType::FuzzExponential2):
      return fuzzExponential2(x);
    break;
    case(WaveShaperType::Exponential2):
      return exponential2(x);
    break;
    case(WaveShaperType::ArctangentSquareRoot):
      return arctangetSquareRoot(x);
    break;
    case(WaveShaperType::SquareSign):
      return squareSign(x);
    break;
    case(WaveShaperType::Cube):
      return cube(x);
    break;
    case(WaveShaperType::HardClipper):
      return hardClipper(x);
    break;
    case(WaveShaperType::HalfWaveRectifier):
      return halfWaveRectifier(x);
    break;
    case(WaveShaperType::FullWaveRectifier):
      return fullWaveRectifier(x);
    break;
    case(WaveShaperType::SquareLaw):
      return squareLaw(x);
    break;
    case(WaveShaperType::AbsoluteSquareRoot):
      return absoluteSquareRoot(x);
    break;
    default:      
      assert(false && "wrong enum value");
      return 0.;
  }  
}


} // namespace ZAudio