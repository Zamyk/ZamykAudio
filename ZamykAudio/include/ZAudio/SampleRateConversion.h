#pragma once

#include <span>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/CircularBuffer.h>
#include <ZAudio/FIR_Filter.h>

namespace ZAudio::Tools {


void convertSampleRateLinearInterpolation(std::span<const sample_t> in, std::span<sample_t> out, Frequency inSampleRate, Frequency outSampleRate);
void convertSampleRateLinearInterpolation(std::span<const sample_t> in, std::span<sample_t> out, size_t inSize, size_t outSize);

void convertSampleRateSinc(std::span<const sample_t> in, std::span<sample_t> out, Frequency inSampleRate, Frequency outSampleRate);

class SampleRateConverter {
public:
  SampleRateConverter() = default;

  SampleRateConverter(Frequency inSampleRate_p, Frequency outSampleRate_p, size_t filterLength = 101);

  void push(sample_t in);

  bool outReady();

  sample_t get();

private:  
  Frequency inSampleRate;
  Frequency outSampleRate;
  FIR_Filter sincLowPass;
  sample_t out = 0.;      
  bool diffrentSampleRates = false;

  double step = 0.;
  double position = 1.;
};


} // namespace ZAudio::Tools