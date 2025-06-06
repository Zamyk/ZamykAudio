#pragma once

#include <span>
#include <numeric>

#include <ZAudio/CommonTypes.h>

namespace ZAudio::Tools {

class WindowFunction  {
public:
enum struct Type {
  None, Hann, Hamming, Blackman
}; 
  static bool isMiddleSymmetric(Type type);
  static double get(Type type, double x);
  static double get(Type type, size_t i, size_t size);
  static void calculateWindow(Type type, std::span<double> window);
  WindowFunction(size_t size, Type type);
  void applyWindow(std::span<sample_t> samples) const;
  void applyGainCorrection(std::span<sample_t> samples) const;
private:
  std::vector<double> window;
  double gainCorrection = 1.;  
};

} // namespace ZAudio::Tools