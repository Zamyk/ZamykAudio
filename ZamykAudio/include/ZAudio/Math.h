#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

namespace ZAudio::Math {

bool isPrime(int64_t v);

double wrapPhase(double phase);

std::vector<int64_t> getFirstN_PrimeNumbers(size_t n);

template<typename T> 
T sgn(T v) {
   return T((T(0) < v) - (v < T(0)));
}

template<typename T>
T dist(T v1, T v2) {
  return std::max(v1, v2) - std::min(v1, v2);
}

double linearInterpolation(double y1, double y2, double f);


} // namespace ZAudio::Math