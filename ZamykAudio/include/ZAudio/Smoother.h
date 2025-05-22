#pragma once

#include <vector>

#include <ZAudio/CommonTypes.h>
#include <ZAudio/Math.h>

namespace ZAudio::Tools {

namespace SmootherConverters {

template<typename T>
double toDouble(T v) {
  return v;
}

template<typename T>
T fromDouble(double d) {
  return d;
}

template<>
inline double toDouble(Frequency v) {
  return v.Hz();
}

template<>
inline Frequency fromDouble(double d) {
  return Frequency::Hz(d);
}

template<>
inline double toDouble(Volume v) {
  return v.dB();
}

template<>
inline Volume fromDouble(double d) {
  return Volume::dB(d);
}

template<>
inline double toDouble(Time v) {
  return v.seconds();
}

template<>
inline Time fromDouble(double d) {
  return Time::seconds(d);
}

} // namespace Smoother

template<typename T>
class Smoother {
public:
  Smoother()= default;
  Smoother(Frequency sampleRate, T from_p, T maxChangePerSecond) :
    curr(SmootherConverters::toDouble(from_p)),
    to(curr),
    step(calculateStep(sampleRate, SmootherConverters::toDouble(maxChangePerSecond))),
    ended(true) {}

  void setDestination(T to_p) {
    to = SmootherConverters::toDouble(to_p);
    ended = false;
  }

  void setInstant(T to_p) {
    to = SmootherConverters::toDouble(to_p);
    ended = true;
    curr = to;
  }

  T update() {
    changed = false;
    if(!ended) {
      if(std::fabs(to - curr) > step) {
        curr += step * (to > curr ? 1 : -1);
        changed = true;
      }
      else {
        curr = to;
        ended = true;
      }
    }
    return SmootherConverters::fromDouble<T>(curr);
  }

  T getCurrentValue() const {
    return SmootherConverters::fromDouble<T>(curr);
  }

  bool hasChanged() const {
    return changed;
  }

private:
  double curr = 0.;
  double to = 0.;
  double step = 0.;
  bool changed = false;
  bool ended = false;

  static double calculateStep(Frequency sampleRate, double maxChangePerSecond) {
    return maxChangePerSecond / sampleRate.Hz();
  }
};


} // namespace ZAudio