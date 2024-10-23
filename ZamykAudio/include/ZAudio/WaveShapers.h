#pragma once

#include <numbers>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/Math.h>


// functions based on Will Pirkle audio-effects page:549
namespace ZAudio::Tools::WaveShapers {

enum struct WaveShaperType {
  Arraya, Sigmoid, Sigmoid2, HyperbolicTangent, ArcTangent, FuzzExponential1, FuzzExponential2, Exponential2, ArctangentSquareRoot,
  SquareSign, Cube, HardClipper, HalfWaveRectifier, FullWaveRectifier, SquareLaw, AbsoluteSquareRoot, 
};

sample_t arraya(sample_t x);
sample_t sigmoid(sample_t x, sample_t k);
sample_t sigmoid2(sample_t x);
sample_t hyperbolicTangent(sample_t x, sample_t k);
sample_t arcTangent(sample_t x, sample_t k);
sample_t fuzzExponential1(sample_t x, sample_t k);
sample_t fuzzExponential2(sample_t x);
sample_t exponential2(sample_t x);
sample_t arctangetSquareRoot(sample_t x);
sample_t squareSign(sample_t x);
sample_t cube(sample_t x);
sample_t hardClipper(sample_t x);
sample_t halfWaveRectifier(sample_t x);
sample_t fullWaveRectifier(sample_t x);
sample_t squareLaw(sample_t x);
sample_t absoluteSquareRoot(sample_t x);
sample_t process(WaveShaperType type, sample_t x, sample_t k = 1.);


} // namespace ZAudio::Tools::WaveShapers