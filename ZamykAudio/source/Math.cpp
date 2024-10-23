#include <ZAudio/Math.h>

#include <vector>
#include <numbers>
#include <cmath>

namespace ZAudio::Math {


bool isPrime(int64_t v) {
  for(int64_t i = 2; i * i <= v; i++) {
    if(v % i == 0) {
      return false;
    }
  }
  return true;
}

double wrapPhase(double phase) {   
  if (phase >= 0) {
    return fmod(phase + std::numbers::pi, 2.0 * std::numbers::pi) - std::numbers::pi;
  }
  else {
    return fmod(phase - std::numbers::pi, -2.0 * std::numbers::pi) + std::numbers::pi;
  }    
}  

std::vector<int64_t> getFirstN_PrimeNumbers(size_t n) {
  std::vector<int64_t> ans;
  ans.reserve(n);

  int64_t i = 2;
  while(ans.size() < n) {
    if(isPrime(i)) {
      ans.push_back(i);
    }
    i++;
  }  
  return ans;
}

double linearInterpolation(double y1, double y2, double f) {
  if(f >= 1.0) {
    return y2;
  }
  return f * y2 + (1.0 - f) * y1;
}


} // namespace ZAudio::Math