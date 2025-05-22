#pragma once

#include <atomic>
#include <span>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/CircularBuffer.h>
#include <ZAudio/FIR_Filter.h>
#include <ZAudio/SoundBuffer.h>

namespace ZAudio::Tools {

namespace SampleRateConversion {
constexpr int32_t DefaultFilterRadius = 50;
}


void convertSampleRateLinearInterpolation(std::span<const sample_t> in, std::span<sample_t> out, Frequency inSampleRate, Frequency outSampleRate);
void convertSampleRateLinearInterpolation(std::span<const sample_t> in, std::span<sample_t> out, size_t inSize, size_t outSize);
SoundBuffer convertSampleRateLinear(const SoundBuffer& in, Frequency outSampleRate);
SoundBuffer convertSampleRateSinc(const SoundBuffer& in, Frequency outSampleRate, int32_t filterRadius = SampleRateConversion::DefaultFilterRadius, std::atomic_bool* interrupt = nullptr);
SoundBuffer changeTempo(const SoundBuffer& in, double tempo, int32_t fitlerRadius = SampleRateConversion::DefaultFilterRadius, std::atomic_bool* interrupt = nullptr);


class SincLookup {
public:
  SincLookup() = default;
  SincLookup(Frequency sampleRate, Frequency cutOffFrequency, uint32_t radius, uint32_t overSample_p, WindowFunction::Type windowFunctionType);
  double get(uint32_t index, double fract) const;
private:
  uint32_t overSample = 1;
  std::vector<double> coefficients;
};

class SincFilter {
public:
  SincFilter() = default;
  SincFilter(Frequency sampleRate, Frequency cutOffFrequency, uint32_t windowSize, WindowFunction::Type windowFunctionType);
  void push(sample_t in);
  sample_t get(double offset) const;
private:
  SincLookup lookup;
  CircularBuffer<sample_t> buffer;
};

class SampleRateConverter {
public:
  SampleRateConverter() = default;

  SampleRateConverter(Frequency inSampleRate_p, Frequency outSampleRate_p, size_t filterLength = 101);
  
  // for live manipulating of frequency, doesn't update filter to prevent audio pauses
  void setOutSampleRateNoFilterUpdate(Frequency outSampleRate_p);

  void push(sample_t in);

  bool outReady();

  sample_t get();

private:
  Frequency inSampleRate;
  Frequency outSampleRate;

  SincFilter filter;  
  sample_t out = 0.;
  bool diffrentSampleRates = false;

  double step = 0.;
  double position = 1.;
};


} // namespace ZAudio::Tools