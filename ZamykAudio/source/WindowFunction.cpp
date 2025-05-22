#include <ZAudio/WindowFunction.h>

#include <numbers>

namespace ZAudio::Tools {

bool WindowFunction::isMiddleSymmetric(Type type) {
  return true;
}

double WindowFunction::get(Type type, double x) {
  if(type == Type::Hann) {
    return 0.5 * (1. - cos(2 * std::numbers::pi * x));
  }
  if(type == Type::Hamming) {
    return 0.54 - 0.46 * cos(2 * std::numbers::pi * x);
  }
  if(type == Type::Blackman) {
    return 0.42 - 0.5 * cos(2. * std::numbers::pi * x) + 0.08 * cos(4. * std::numbers::pi * x);
  }
  assert(false);
  return 0.;
}

double WindowFunction::get(Type type, size_t i, size_t size) {
  return get(type, static_cast<double>(i) / static_cast<double>(size));
}

void WindowFunction::calculateWindow(Type type, std::span<double> window) {
  for(size_t i = 0; i < window.size(); i++) {
    window[i] = get(type, i, window.size());
  }
}

WindowFunction::WindowFunction(size_t size, Type type) : window(size) {
  calculateWindow(type, window);
  double sum = 0.;
  for(auto v : window) {
    sum += v;
  }
  gainCorrection = size / sum;
}

void WindowFunction::applyWindow(std::span<sample_t> samples) const {
  assert(window.size() == samples.size());
  for(size_t i = 0; i < window.size(); i++) {
    samples[i] *= window[i];
  }
}

void WindowFunction::applyGainCorrection(std::span<sample_t> samples) const {
  for(auto& sample : samples) {
    sample *= gainCorrection;
  }
}


} // namespace ZAudio::Tools