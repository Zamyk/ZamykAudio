#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/WindowFunction.h>
#include <ZAudio/CircularBuffer.h>

namespace ZAudio::Tools {


class FIR_Filter {
public:
  FIR_Filter() = default;
  explicit FIR_Filter(const std::vector<double>& coefficients_p);
  static FIR_Filter sincFilter(Frequency sampleRate, Frequency cutOffFrequency, uint32_t windowSize, WindowFunction::Type windowFunctionType);
  sample_t process(sample_t in);
private:
  std::vector<double> coefficients;
  CircularBuffer<sample_t> buffer;
};


} // namespace ZAudio::Tools