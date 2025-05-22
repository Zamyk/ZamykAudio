#include <ZAudio/FFT.h>

#ifdef ZAUDIO_USE_FFT

#include <fftw3.h>

namespace ZAudio::Tools {



FFT::FFT(size_t frameSize_p) 
  : frameSize(frameSize_p), samples(frameSize), bins(frameSize)
{
  const std::lock_guard<std::mutex> lock(mutex);    
  fftwSamples = static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * 2 * frameSize));
  fftwBins = static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * 2 * frameSize));  
  fftPlan = fftw_plan_dft_1d(samples.size(), fftwSamples, fftwBins, FFTW_FORWARD, FFTW_ESTIMATE);
  inverseFftPlan = fftw_plan_dft_1d(samples.size(), fftwSamples, fftwBins, FFTW_BACKWARD, FFTW_ESTIMATE);
}



FFT::FFT(FFT&& oth) noexcept {
  fftw_free(fftwSamples);
  fftw_free(fftwBins);
  frameSize = oth.frameSize;
  samples = std::move(oth.samples);
  bins = std::move(oth.bins);
  fftwSamples = oth.fftwSamples;
  fftwBins = oth.fftwBins;
  oth.fftwSamples = nullptr;
  oth.fftwBins = nullptr;  

  if(fftPlan || inverseFftPlan) {
    const std::lock_guard<std::mutex> lock(mutex);
    if(fftPlan) {
      fftw_destroy_plan(fftPlan);
    }
    if (inverseFftPlan) {
      fftw_destroy_plan(inverseFftPlan);  
    }
    fftPlan = nullptr;
    inverseFftPlan = nullptr;
  }
  fftPlan = oth.fftPlan;
  inverseFftPlan = oth.inverseFftPlan;

  oth.fftPlan = nullptr;
  oth.inverseFftPlan = nullptr;
}  

FFT& FFT::operator= (FFT&& oth) noexcept {
  fftw_free(fftwSamples);
  fftw_free(fftwBins);
  frameSize = oth.frameSize;
  samples = std::move(oth.samples);
  bins = std::move(oth.bins);
  fftwSamples = oth.fftwSamples;
  fftwBins = oth.fftwBins;
  oth.fftwSamples = nullptr;
  oth.fftwBins = nullptr;  

  if(fftPlan || inverseFftPlan) {
    const std::lock_guard<std::mutex> lock(mutex);
    if(fftPlan) {
      fftw_destroy_plan(fftPlan);
    }
    if (inverseFftPlan) {
      fftw_destroy_plan(inverseFftPlan);  
    }
    fftPlan = nullptr;
    inverseFftPlan = nullptr;
  }
  fftPlan = oth.fftPlan;
  inverseFftPlan = oth.inverseFftPlan;
  
  oth.fftPlan = nullptr;
  oth.inverseFftPlan = nullptr;
  return *this;
}

FFT::~FFT() {
  if(fftPlan || inverseFftPlan) {
    const std::lock_guard<std::mutex> lock(mutex);
    if(fftPlan) {
      fftw_destroy_plan(fftPlan);
    }
    if (inverseFftPlan) {
      fftw_destroy_plan(inverseFftPlan);  
    }    
  }
  fftw_free(fftwSamples);
  fftw_free(fftwBins);  
}

void FFT::doFFT() {  
  for(size_t i = 0; i < frameSize; i++) {
    fftwSamples[i][0] = samples[i];
    fftwSamples[i][1] = 0.;
  }  
  fftw_execute(fftPlan);  

  for(size_t i = 0; i < bins.size(); i++) {
    bins[i].real(fftwBins[i][0]);
    bins[i].imag(fftwBins[i][1]);    
    //bins[i] = std::polar(std::abs(bins[i]) / frameSize, std::arg(bins[i]));
    bins[i] /= frameSize;
  }  
}

void FFT::doInverseFFT() {
  for(size_t i = 0; i < frameSize; i++) {
    fftwSamples[i][0] = bins[i].real();
    fftwSamples[i][1] = bins[i].imag();    
  }
    
  fftw_execute(inverseFftPlan);

  for(size_t i = 0; i < bins.size(); i++) {
    samples[i] = fftwBins[i][0];
  }    
}

std::vector<sample_t>& FFT::getSamples() {
  return samples;
}

const std::vector<sample_t>& FFT::getSamples() const {
  return samples;
}

std::vector<std::complex<double>>& FFT::getBins() {
  return bins;
}

const std::vector<std::complex<double>>& FFT::getBins() const {
  return bins;
}


} // namespace ZAudio::Tools

#else

namespace ZAudio::Tools {

FFT::FFT(size_t frameSize_p) 
  : frameSize(frameSize_p), samples(frameSize), bins(frameSize)
{
}



FFT::FFT(FFT&& oth) noexcept {
}  

FFT& FFT::operator= (FFT&& oth) noexcept {
  return *this;
}

FFT::~FFT() {
}

void FFT::doFFT() {  
  assert(false);  
}

void FFT::doInverseFFT() {
  assert(false);
}

std::vector<sample_t>& FFT::getSamples() {
  assert(false);
  return samples;
}

const std::vector<sample_t>& FFT::getSamples() const {
  assert(false);
  return samples;
}

std::vector<std::complex<double>>& FFT::getBins() {
  assert(false);
  return bins;
}

const std::vector<std::complex<double>>& FFT::getBins() const {
  assert(false);
  return bins;
}

} // namespace

#endif