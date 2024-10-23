#pragma once

#include <string>
#include <array>

#include <ZAudio/CircularBuffer.h>
#include <ZAudio/CommonTypes.h>

namespace ZAudio::Tools {


class AnalogFilter {
public:  
enum struct Type {
  ByPass, AllPass, LowPass, HighPass, FirstOrderLowPass, FirstOrderHighPass, LowShelf, HighShelf, BandPass, BandStop
};
struct Parameters {
  Parameters() = default;
  Parameters(Type type_p, Frequency sampleRate_p, Frequency frequency_p, double q_p = 0., Volume gain_p = Volume::dB(0));

  static Parameters createByPassParameters();
  static Parameters createAllPassParameters(Frequency sampleRate, Frequency frequency);
  static Parameters createFirstOrderLowPassParameters(Frequency sampleRate, Frequency frequency);
  static Parameters createFirstOrderHighPassfParameters(Frequency sampleRate, Frequency frequency);
  static Parameters createLowPassParameters(Frequency sampleRate, Frequency frequency, double q = 0.707);
  static Parameters createHighPassfParameters(Frequency sampleRate, Frequency frequency, double q = 0.707);
  static Parameters createHighShelfParameters(Frequency sampleRate, Frequency frequency, Volume gain);
  static Parameters createLowShelfParameters(Frequency sampleRate, Frequency frequency, Volume gain);  
  static Parameters createBandPassParameters(Frequency sampleRate, Frequency frequency, double q);
  static Parameters createBandStopParameters(Frequency sampleRate, Frequency frequency, double q);

  Type type = Type::ByPass;
  Frequency sampleRate;
  Frequency frequency;
  Volume gain;
  double q = 0.707;  
};  
  static constexpr uint32_t NumOfFilterTypes = 10;
  static const inline std::array<std::pair<Type, std::string>, NumOfFilterTypes> typesToStrings = {
    std::make_pair(Type::ByPass, "ByPass"),
    std::make_pair(Type::AllPass, "AllPass"),
    std::make_pair(Type::LowPass, "LowPass"),
    std::make_pair(Type::HighPass, "HighPass"),
    std::make_pair(Type::FirstOrderLowPass, "FirstOrderLowPass"),
    std::make_pair(Type::FirstOrderHighPass, "FirstOrderHighPass"),
    std::make_pair(Type::LowShelf, "LowShelf"),
    std::make_pair(Type::HighShelf, "HighShelf"),
    std::make_pair(Type::BandPass, "BandPass"),
    std::make_pair(Type::BandStop, "BandStop")
  };

  AnalogFilter();
  explicit AnalogFilter(Parameters parameters);
  void reset(Parameters parameters);
  sample_t process(sample_t in);

private:
  double a0 = 1;
  double a1 = 0;
  double a2 = 0;
  double b1 = 0;
  double b2 = 0;

  CircularBuffer<sample_t> aDelay; 
  CircularBuffer<sample_t> bDelay;

  void setByPass();
  void setAllPass(Frequency sampleRate, Frequency frequency);
  void setFirstOrderLowPass(Frequency sampleRate, Frequency frequency);
  void setFirstOrderHighPass(Frequency sampleRate, Frequency frequency);
  void setLowPass(Frequency sampleRate, Frequency frequency, double q);
  void setHighPass(Frequency sampleRate, Frequency frequency, double q);
  void setLowShelf(Frequency sampleRate, Frequency frequency, Volume gain);
  void setHighShelf(Frequency sampleRate, Frequency frequency, Volume gain);
  void setBandPass(Frequency sampleRate, Frequency frequency, double q);
  void setBandStop(Frequency sampleRate, Frequency frequency, double q);
};



} // namespace ZAudio::Tools