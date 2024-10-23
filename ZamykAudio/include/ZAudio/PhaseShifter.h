#pragma once

#include <array>
#include <ZAudio/ModulatedDelay.h>
#include <ZAudio/AnalogFilter.h>

namespace ZAudio::Tools {


class PhaseShifter {
public:  
  PhaseShifter() = default;
  PhaseShifter(Frequency sampleRate_p, double depth_p, Frequency rate_p, double phaseOffset_p);

  void setDepth(double depth_p);
  void setRate(Frequency rate_p);  

  sample_t process(sample_t in);

private:
  Frequency sampleRate;
  double depth = 1.;  
  LowFrequencyOscillator lfo;
  static constexpr size_t NumOfFilters = 6;
  std::array<Frequency, NumOfFilters> minFrequencies = {Frequency::Hz(16), Frequency::Hz(33), Frequency::Hz(48), Frequency::Hz(98), Frequency::Hz(160), Frequency::Hz(260)};
  std::array<Frequency, NumOfFilters> maxFrequencies = {Frequency::Hz(1600), Frequency::Hz(3300), Frequency::Hz(4800), Frequency::Hz(9800), Frequency::Hz(16000), Frequency::Hz(20480)};
  std::array<AnalogFilter, NumOfFilters> filters;  
};




} // namespace ZAudio::Tools