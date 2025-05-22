#include <ZAudio/PhaseVocoder.h>
#include <numbers>

namespace ZAudio::Tools {


// PhaseCorrector -------------------------------------------------------------------------------------------------------------------

PhaseCorrector::Peak::Peak(size_t position_p, size_t from_p, size_t to_p) : position(position_p), from(from_p), to(to_p) {}

PhaseCorrector::PhaseCorrector(size_t frameSize_p, size_t inputHopSize_p, size_t outputHopSize_p, Algorithm phaseCorrectionAlgorithm_p) : 
  frameSize(frameSize_p),
  inputHopSize(inputHopSize_p), 
  outputHopSize(outputHopSize_p),
  stretchRatio(static_cast<double>(outputHopSize) / static_cast<double>(inputHopSize)),
  phaseCorrectionAlgorithm(phaseCorrectionAlgorithm_p),
  prevPhase(frameSize),     
  prevCorrectedPhase(frameSize)  
{
  peaks.reserve(frameSize / 2);
}

void PhaseCorrector::operator() (std::vector<std::complex<double>>& bins) {   
  switch (phaseCorrectionAlgorithm) {
    case Algorithm::Normal:
      normalCorrect(bins);        
    break;

    case Algorithm::PhaseLock:
      phaseLockCorrect(bins);        
    break;

    case Algorithm::PhaseTrack:
      phaseTrackCorrect(bins);
    break;      

    default:
    break;
  }
}

bool PhaseCorrector::isPeak(size_t position, const std::vector<std::complex<double>>& bins) {
  for(int32_t i = std::max(0, static_cast<int32_t>(position) - 2); i <= std::min<int32_t>(position + 2, frameSize - 1); i++) {
    if(std::abs(bins[i]) > std::abs(bins[position])) {
      return false;
    }
  }
  return true;
}

void PhaseCorrector::findPeaks(const std::vector<std::complex<double>>& bins) {
  peaks.clear();

  // add peaks
  for(size_t i = 0; i < frameSize; i++) {
    if(isPeak(i, bins)) {
      peaks.push_back(Peak(i, i, i));
    }
  }

  // add influenced area
  peaks.front().from = 0;
  peaks.back().to = frameSize - 1;
  for(size_t i = 0; i < peaks.size() - 1; i++) {
    size_t mid = peaks[i].position + (peaks[i + 1].position - peaks[i].position) / 2;
    peaks[i].to = mid;
    peaks[i + 1].from = mid + 1;
  }
}

double PhaseCorrector::getPhaseIncrement(const std::vector<std::complex<double>>& bins, size_t i) {    
  const double binFrequency = 2. * std::numbers::pi * i / static_cast<double>(frameSize);
  const double currPhase = std::arg(bins[i]);    
  const double phaseDeviation = currPhase - prevPhase[i] - inputHopSize * binFrequency;
  const double phaseIncrement = inputHopSize * binFrequency + Math::wrapPhase(phaseDeviation);
  return phaseIncrement;
}

void PhaseCorrector::correctSingleBinPhase(const std::vector<std::complex<double>>& bins, size_t i) {      
  const double phaseIncrement = getPhaseIncrement(bins, i);    
  prevCorrectedPhase[i] = Math::wrapPhase(prevCorrectedPhase[i] + phaseIncrement * stretchRatio);    
}

void PhaseCorrector::normalCorrect(std::vector<std::complex<double>>& bins) {    
  for(size_t i = 0; i < frameSize; i++) {
    correctSingleBinPhase(bins, i);

    // update for next frame
    prevPhase[i] = std::arg(bins[i]);
    bins[i] = std::polar(std::abs(bins[i]), prevCorrectedPhase[i]);      
  }        
}

void PhaseCorrector::phaseLockCorrect(std::vector<std::complex<double>>& bins) {
  findPeaks(bins);        

  for(size_t i = 0; i < frameSize; i++) {
    correctSingleBinPhase(bins, i);      
    prevPhase[i] = std::arg(bins[i]);
    bins[i] = std::polar(std::abs(bins[i]), prevCorrectedPhase[i]);      
  }

  // for every peak, update all influenced bins with locked phase
  for(const auto& peak : peaks) {
    for(size_t i = peak.from; i <= peak.to; i++) {
      if(i != peak.position) {                    
        // we use prevPhase[peak.position], because bins is already updated => == prevCorrectedPhase
        bins[i] = std::polar(std::abs(bins[i]), Math::wrapPhase(prevCorrectedPhase[peak.position] + prevPhase[peak.position] + prevPhase[i])); //FIXME make sure that it should be + prevPhase[i]
      }
    }
  }
}  

void PhaseCorrector::phaseTrackCorrect(std::vector<std::complex<double>>& bins) {    
  findPeaks(bins);
  int32_t l = 0;
  for(auto peak : peaks) {            
    while(l + 1 < static_cast<int32_t>(prevPeaks.size()) && Math::dist(peak.position, prevPeaks[l].position) > Math::dist(peak.position, prevPeaks[l + 1].position)) {
      l++;
    }
    if(prevPeaks.size() && Math::dist(peak.position, prevPeaks[l].position) < 4) {
      const double phaseIncrement = getPhaseIncrement(bins, peak.position);
      prevCorrectedPhase[peak.position] = Math::wrapPhase(prevCorrectedPhase[prevPeaks[l].position] + phaseIncrement * stretchRatio);
      l++;
    }
    else {
      correctSingleBinPhase(bins, peak.position);
    }
    prevPhase[peak.position] = std::arg(bins[peak.position]);
    bins[peak.position] = std::polar(std::abs(bins[peak.position]), prevCorrectedPhase[peak.position]);
  }

  // for every peak, update all influenced bins with locked phase
  for(const auto& peak : peaks) {
    for(size_t i = peak.from; i <= peak.to; i++) {
      if(i != peak.position) {                    
        correctSingleBinPhase(bins, i);      
        prevPhase[i] = std::arg(bins[i]);                    
        bins[i] = std::polar(std::abs(bins[i]), Math::wrapPhase(prevCorrectedPhase[peak.position] + prevPhase[peak.position] + prevPhase[i])); //FIXME make sure that it should be + prevPhase[i]
      }
    }
  }
  prevPeaks = peaks;
}

void PhaseCorrector::setInputHopSize(size_t inputHopSize_p) {
  inputHopSize = inputHopSize_p;
  stretchRatio = (static_cast<double>(outputHopSize) / static_cast<double>(inputHopSize));
}

void PhaseCorrector::setOutputHopSize(size_t outputHopSize_p) {
  outputHopSize = outputHopSize_p;
  stretchRatio = (static_cast<double>(outputHopSize) / static_cast<double>(inputHopSize));
}

void PhaseCorrector::setAlgorithm(Algorithm algorithm) {
  phaseCorrectionAlgorithm = algorithm;
}

// PhaseVocoder ---------------------------------------------------------------------------------------------------------------------

PhaseVocoder::PhaseVocoder(size_t frameSize_p, size_t inputHopSize_p, size_t outputHopSize_p, PhaseCorrector::Algorithm phaseCorrectionAlgorithm_p) : 
  frameSize(frameSize_p),
  inputHopSize(inputHopSize_p),
  outputHopSize(outputHopSize_p),
  inputFrame(frameSize),
  outputFrame(frameSize),
  fft(frameSize),  
  window(frameSize, WindowFunction::Type::Hann),
  phaseCorrector(frameSize, inputHopSize, outputHopSize, phaseCorrectionAlgorithm_p) {}
  
PhaseVocoder::PhaseVocoder(size_t frameSize_p, size_t hopSize) : PhaseVocoder(frameSize_p, hopSize, hopSize, PhaseCorrector::Algorithm::None) {}

void PhaseVocoder::push(sample_t in) {    
  frameComplete = false;

  inputFrame.push(in);
  numOfInputSamples++;

  // fft frame is ready  
  if(numOfInputSamples == frameSize) {
    frameComplete = true;
    ifftDone = false;
    numOfInputSamples -= inputHopSize;
    auto& fftSamples = fft.getSamples();

    for(size_t i = 0; i < frameSize; i++) {                
      fftSamples[i] = inputFrame.get(frameSize - i - 1); // cycle buffer is "reversed", thats why we use frameSize - i - 1 instead of i
    }      
    window.applyWindow(fftSamples);
    fft.doFFT();                                   
  }    
}

sample_t PhaseVocoder::get() {
  if(!ifftDone) {
    doInverseFFT();
  }            
  sample_t out = outputFrame.get(frameSize - 1);
  outputFrame.push(0.);
  return out;
}     

void PhaseVocoder::doInverseFFT() {    
  correctPhases();
  fft.doInverseFFT();                

  auto& fftSamples = fft.getSamples();
  window.applyWindow(fftSamples);
  window.applyGainCorrection(fftSamples);
  window.applyGainCorrection(fftSamples);

  auto it = outputFrame.current();

  const double Div = static_cast<double>(frameSize) / static_cast<double>(outputHopSize);      
  for(size_t i = 0; i < frameSize; i++) {
    *it += fftSamples[frameSize - i - 1] / Div;
    it++;  
  }    
  ifftDone = true;
}

bool PhaseVocoder::frameCompleted() const {
  return frameComplete;
}

std::vector<std::complex<double>>& PhaseVocoder::getBins() {
  return fft.getBins();
}

void PhaseVocoder::correctPhases() {
  auto& bins = fft.getBins();
  phaseCorrector(bins);    
}

void PhaseVocoder::setInputHopSize(size_t inputHopSize_p) {
  inputHopSize = inputHopSize_p;  
  phaseCorrector.setInputHopSize(inputHopSize);
}

void PhaseVocoder::setOutputHopSize(size_t outputHopSize_p) {
  outputHopSize = outputHopSize_p;  
  phaseCorrector.setOutputHopSize(outputHopSize);
}

void PhaseVocoder::setPhaseCorrectionAlgorithm(PhaseCorrector::Algorithm algorithm) {
  phaseCorrector.setAlgorithm(algorithm);
}

} // namespace ZAudio::Tools