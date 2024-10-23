#pragma once

#include <span>
#include <complex>
#include <vector>
#include <thread>
#include <mutex>

#include <ZAudio/CommonTypes.h>

typedef double fftw_complex[2];
struct fftw_plan_s;             // forward
typedef fftw_plan_s *fftw_plan;

namespace ZAudio::Tools {

class FFT {
public:

  explicit FFT(size_t frameSize_p);

  FFT(const FFT& oth) = delete;
  FFT(FFT&& oth) noexcept;
  FFT& operator= (const FFT& oth) = delete;
  FFT& operator= (FFT&& oth) noexcept;
  
  ~FFT();

  void doFFT();
  void doInverseFFT();
  
  std::vector<sample_t>& getSamples();
  const std::vector<sample_t>& getSamples() const;
  std::vector<std::complex<double>>& getBins();
  const std::vector<std::complex<double>>& getBins() const;

private:
  size_t frameSize = 0;        

  std::vector<sample_t> samples;
  std::vector<std::complex<double>> bins;
  
  fftw_complex* fftwSamples = nullptr;
  fftw_complex* fftwBins= nullptr;

  fftw_plan fftPlan = nullptr;
  fftw_plan inverseFftPlan = nullptr;

  inline static std::mutex mutex;
};

} // namespace