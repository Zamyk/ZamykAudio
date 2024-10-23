#include <ZAudio/SampleRateConversion.h>
#include <ZAudio/Math.h>

#include <iostream>


namespace ZAudio::Tools {


void convertSampleRateLinearInterpolation(std::span<const sample_t> in, std::span<sample_t> out, Frequency inSampleRate, Frequency outSampleRate) {
  const double Mul = outSampleRate.Hz() / inSampleRate.Hz();

  for(size_t i = 0; i < out.size(); i++) {
    size_t ind1 = static_cast<size_t>(i * Mul);
    size_t ind2 = static_cast<size_t>(i * Mul + 1.);
    if(ind2 >= in.size()) {
      ind2 = ind1;
    }        
    out[i] = Math::linearInterpolation(in[ind1], in[ind2], i * Mul - static_cast<double>(ind1));
  }
}

void convertSampleRateLinearInterpolation(std::span<const sample_t> in, std::span<sample_t> out, size_t inSize, size_t outSize) {  
  const double Mul = static_cast<double>(inSize) / static_cast<double>(outSize);

  for(size_t i = 0; i < out.size(); i++) {
    size_t ind1 = static_cast<size_t>(i * Mul);
    size_t ind2 = static_cast<size_t>(i * Mul + 1.);
    if(ind2 >= in.size()) {
      ind2 = ind1;
    }        
    out[i] = Math::linearInterpolation(in[ind1], in[ind2], i * Mul - static_cast<double>(ind1));
  }
}

void convertSampleRateSinc(std::span<const sample_t> in, std::span<sample_t> out, Frequency inSampleRate, Frequency outSampleRate) {
  if(inSampleRate == outSampleRate) {
    for(size_t i = 0; i < std::min(in.size(), out.size()); i++) {
      out[i] = in[i];
    }
  }
  else {
    SampleRateConverter converter(inSampleRate, outSampleRate);
    size_t j = 0;
    for(size_t i =  0; i < in.size() && j < out.size(); i++) {
      converter.push(in[i]);
      while(converter.outReady() && j < out.size()) {
        out[j] = converter.get();
        j++;
      }
    }  
  }
}

// SampeRateConverter-----------------------------------------------------------------------

SampleRateConverter::SampleRateConverter(Frequency inSampleRate_p, Frequency outSampleRate_p, size_t filterLength) :
  inSampleRate(inSampleRate_p),
  outSampleRate(outSampleRate_p),                
  step(inSampleRate_p / outSampleRate_p),
  diffrentSampleRates(inSampleRate != outSampleRate)
{
  if(diffrentSampleRates) {
    sincLowPass = FIR_Filter::sincFilter(inSampleRate, std::min(inSampleRate, outSampleRate) / 2., filterLength, WindowFunction::Type::Blackman);
  }
}

void SampleRateConverter::push(sample_t in) {
  position -= 1.;
  if(diffrentSampleRates) {
    out = sincLowPass.process(in);    
  }
  else {
    out = in;
  }
}

bool SampleRateConverter::outReady() {
  return position < 1.;    
}

sample_t SampleRateConverter::get() {    
  if(!outReady()) {
    return 0.;
  }
  position += step;    
  return out;
}


} // namespace ZAudio::Tools