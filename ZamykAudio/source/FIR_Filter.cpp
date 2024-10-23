#include <ZAudio/FIR_Filter.h>
#include <numbers>

namespace ZAudio::Tools {


FIR_Filter::FIR_Filter(const std::vector<double>& coefficients_p) : coefficients(coefficients_p) {}

FIR_Filter FIR_Filter::sincFilter(Frequency sampleRate, Frequency cutOffFrequency, uint32_t windowSize, WindowFunction::Type windowFunctionType) {
  if(windowSize % 2 == 0) {
    windowSize++;
  }

  FIR_Filter filter;
  filter.coefficients.resize(windowSize);
  filter.buffer.resize(windowSize);
  WindowFunction::calculateWindow(windowFunctionType, filter.coefficients);

  const double fc = cutOffFrequency.Hz() / sampleRate.Hz();
  double sum = 0.;  

  for(int32_t i = 0; i < windowSize; i++) {
    if(i - windowSize / 2 == 0) {
      filter.coefficients[i] *= 2 * std::numbers::pi * fc;
    }
    else {
      filter.coefficients[i] *= sin(2. * std::numbers::pi * fc * (i - windowSize / 2)) / (i - windowSize / 2);
    }    
    sum += filter.coefficients[i];
  }
  const double gainCorrection = 1 / sum;
  for(auto& v : filter.coefficients) {
    v *= gainCorrection;
  }

  return filter;    
}

sample_t FIR_Filter::process(sample_t in) {
  buffer.push(in);
  sample_t out = 0.;
  for(size_t i = 0; i < coefficients.size(); i++) {
    out += coefficients[i] * buffer.get(i);
  }
  return out;
}



} // namespace ZAudio::Tools