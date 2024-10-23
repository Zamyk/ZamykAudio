#pragma once

#include <ZAudio/FFT.h>
#include <ZAudio/CircularBuffer.h>
#include <ZAudio/WindowFunction.h>
#include <ZAudio/Math.h>

namespace ZAudio::Tools {

class PhaseCorrector {
public:
enum struct Algorithm {
  None, Normal, PhaseLock, PhaseTrack
};
  PhaseCorrector(size_t frameSize_p, size_t inputHopSize_p, size_t outputHopSize_p, Algorithm phaseCorrectionAlgorithm_p);
  void operator() (std::vector<std::complex<double>>& bins);

  void setInputHopSize(size_t inputHopSize_p);
  void setOutputHopSize(size_t outputHopSize_p);
  void setAlgorithm(Algorithm algorithm);
private:  
  struct Peak {
    size_t position = 0;
    size_t from = 0;
    size_t to = 0;

    Peak() = default;
    Peak(size_t position_p, size_t from_p, size_t to_p);
  };

  size_t frameSize = 0;
  size_t inputHopSize = 0;
  size_t outputHopSize = 0;
  double stretchRatio = 1.;
  Algorithm phaseCorrectionAlgorithm;

  std::vector<double> prevPhase;
  std::vector<double> prevCorrectedPhase;  
  std::vector<Peak> peaks;
  std::vector<Peak> prevPeaks;

  bool isPeak(size_t position, const std::vector<std::complex<double>>& bins);
  void findPeaks(const std::vector<std::complex<double>>& bins);  
  double getPhaseIncrement(const std::vector<std::complex<double>>& bins, size_t i);

  // sets prevCorrectedPhase according to noraml phase deviation correction
  void correctSingleBinPhase(const std::vector<std::complex<double>>& bins, size_t i);

  void normalCorrect(std::vector<std::complex<double>>& bins);
  void phaseLockCorrect(std::vector<std::complex<double>>& bins);
  void phaseTrackCorrect(std::vector<std::complex<double>>& bins);    
};

class PhaseVocoder {
public:  
  PhaseVocoder(size_t frameSize_p, size_t inputHopSize_p, size_t outputHopSize_p, PhaseCorrector::Algorithm phaseCorrectionAlgorithm_p);
  PhaseVocoder(size_t frameSize_p, size_t hopSize);
  void push(sample_t in);
  sample_t get();    
  bool frameCompleted() const;
  std::vector<std::complex<double>>& getBins();

  void setInputHopSize(size_t inputHopSize_p);
  void setOutputHopSize(size_t outputHopSize_p);
  void setPhaseCorrectionAlgorithm(PhaseCorrector::Algorithm algorithm);
private:
  size_t frameSize = 0;
  size_t inputHopSize = 0;  
  size_t outputHopSize = 0;  

  size_t numOfInputSamples = 0;
  size_t outputSampleInd = 0;

  CircularBuffer<sample_t> inputFrame;
  CircularBuffer<sample_t> outputFrame;  

  FFT fft;
  WindowFunction window;  
  
  PhaseCorrector phaseCorrector;  

  bool frameComplete = false;   
  bool ifftDone = true;

  void doInverseFFT();
  void correctPhases();
};

} // namespace ZAudio::Tools