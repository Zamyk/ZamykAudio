#include <ZAudio/WindowFunction.h>

#include <numbers>

namespace ZAudio::Tools {


void WindowFunction::calculateWindow(Type type, std::span<double> window) {
  if(type == Type::Hann) {
    for(size_t i = 0; i < window.size(); i++) {
      window[i] = 0.5 * (1. - cos(2 * std::numbers::pi * i / window.size()));
    }
  }     
  if(type == Type::Hamming) {
    for(size_t i = 0; i < window.size(); i++) {
      window[i] = 0.54 - 0.46 * cos(2 * std::numbers::pi * i / window.size());
    }
  }
  if(type == Type::Blackman) {
    for(size_t i = 0; i < window.size(); i++) {
      window[i] = 0.42 - 0.5 * cos(2. * std::numbers::pi * i / window.size()) + 0.08 * cos(4. * std::numbers::pi * i / window.size());
    }
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