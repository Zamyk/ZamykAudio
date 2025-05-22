#include <ZAudio/SampleRateConversion.h>
#include <ZAudio/Math.h>

#include <iostream>
#include <numbers>


namespace ZAudio::Tools {


void convertSampleRateLinearInterpolation(std::span<const sample_t> in, std::span<sample_t> out, Frequency inSampleRate, Frequency outSampleRate) {
  convertSampleRateLinearInterpolation(in, out, in.size(), in.size() * (outSampleRate.Hz() / inSampleRate.Hz()));
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

SoundBuffer convertSampleRateLinear(const SoundBuffer& in, Frequency outSampleRate) {
  if(in.getSampleRate() == outSampleRate) {
    return in;
  }
  const double ratio = (outSampleRate / in.getSampleRate());
  SoundBuffer out(outSampleRate, in.getFrameFormat(), in.getLength() * ratio, in.getLoopStart() * ratio, in.getLoopEnd() * ratio);

  const double step = in.getSampleRate() / outSampleRate;
  double position = 0.;

  for(size_t i = 0; i < out.getLength(); i++) {
    for(size_t channel = 0; channel < out.getNumberOfChannels(); channel++) {
      size_t ind1 = static_cast<size_t>(position);
      size_t ind2 = static_cast<size_t>(position + 1.);
      sample_t s1 = (ind1 < in.getLength() ? in.getSample(ind1, channel) : 0.);
      sample_t s2 = (ind2 < in.getLength() ? in.getSample(ind2, channel) : 0.);
      out.setSample(i, channel, Math::linearInterpolation(s1, s2, position - ind1));
    }
    position += step;
  }
  return out;
}

static sample_t get(std::span<const sample_t> in, const SincLookup& lookup, int32_t radius, double position) {
  sample_t ans = 0.;
  int32_t index = std::floor(position);
  double fract = position - index;

  for(int32_t d = 0; d <= std::min(radius, index); d++) {
    int32_t i = index - d;
    sample_t x = 0.;
    //if(i >= 0 && i < static_cast<int32_t>(in.getLength())) {
      x = in[i];
    //}
    ans += x * lookup.get(d, fract);
  }

  fract = 1. - fract;

  for(int32_t d = 0; d < std::min(radius, static_cast<int32_t>(in.size()) - index - 1); d++) {
    int32_t i = index + 1 + d;
    sample_t x = 0.;
    //if(i >= 0 && i < static_cast<int32_t>(in.getLength())) {
      x = in[i];
    //}
    ans += x * lookup.get(d, fract);
  }

  return ans;
}

SoundBuffer convertSampleRateSinc(const SoundBuffer& in, Frequency outSampleRate, int32_t filterRadius, std::atomic_bool* interrupt) {
  if(in.getSampleRate() == outSampleRate) {
    return in;
  }
  const double ratio = (outSampleRate / in.getSampleRate());
  SoundBuffer out(outSampleRate, in.getFrameFormat(), in.getLength() * ratio, in.getLoopStart() * ratio, in.getLoopEnd() * ratio);
  SincLookup lookup(in.getSampleRate(), std::min(in.getSampleRate(), outSampleRate) / 2., filterRadius + 2, 4, WindowFunction::Type::Blackman);

  const double step = in.getSampleRate() / outSampleRate;
  for(size_t channel = 0; channel < out.getNumberOfChannels(); channel++) {
    double position = 0.;
    auto inSamples = in.getChannel(channel);
    auto outSamples = out.getChannel(channel);
    for(size_t i = 0; i < outSamples.size(); i++) {
      static constexpr size_t CheckInterruptPeriod = 256;
      if(interrupt != nullptr && i % CheckInterruptPeriod == 0) {
        if(*interrupt) {
          return out;
        }
      }
      //out.setSample(i, channel, get(in, channel, lookup, filterRadius, position));
      outSamples[i] = get(inSamples, lookup, filterRadius, position);
      position += step;
    }
  }
  return out;
}

SoundBuffer changeTempo(const SoundBuffer& in, double tempo, int32_t filterRadius, std::atomic_bool* interrupt) {
  if(tempo == 1.) {
    return in;
  }
  auto tmp = ZAudio::Tools::convertSampleRateSinc(in, in.getSampleRate() / tempo, filterRadius, interrupt);
  tmp.setSampleRate(in.getSampleRate());
  return tmp;
}

// SincLookup-------------------------------------------------------------------------------

SincLookup::SincLookup(Frequency sampleRate, Frequency cutOffFrequency, uint32_t radius, uint32_t overSample_p, WindowFunction::Type windowFunctionType) :
  overSample(overSample_p),
  coefficients( overSample * (radius + 1) )
{
  assert(WindowFunction::isMiddleSymmetric(windowFunctionType));

  const double fc = cutOffFrequency.Hz() / sampleRate.Hz();

  // the window is centered at 0.5, so we want values from range <0.5, 1>
  coefficients[0] = 2. * std::numbers::pi * fc * WindowFunction::get(windowFunctionType, 0.5);
  double sum = coefficients[0];

  for(size_t i = 1; i < coefficients.size(); i++) {
    double x = static_cast<double>(i) / overSample;
    const double sinc = sin(2. * std::numbers::pi * fc * x) / x;
    coefficients[i] = WindowFunction::get(windowFunctionType, 0.5 + 0.5 * static_cast<double>(i) / static_cast<double>(coefficients.size() - 1)) * sinc;
    sum += coefficients[i];
  }

  // 0.5 - its only half of filter
  const double normalizationFactor = 0.5 * overSample / sum;
  for(auto& v : coefficients) {
    v *= normalizationFactor;
  }
}

double SincLookup::get(uint32_t index, double fract) const {
  uint32_t sub = static_cast<uint32_t>(fract * overSample);
  index = index * overSample + sub;
  fract = fract * overSample - sub;
  return Math::linearInterpolation(coefficients[index], coefficients[index + 1], fract);
}

// SincFilter-------------------------------------------------------------------------------

static size_t nextOdd(size_t i) {
  return i + 1 - i % 2;
}

static constexpr size_t SincOverSample = 4;

SincFilter::SincFilter(Frequency sampleRate, Frequency cutOffFrequency, uint32_t windowSize, WindowFunction::Type windowFunctionType) :
  lookup(sampleRate, cutOffFrequency, windowSize / 2 + 1, SincOverSample, windowFunctionType),
  buffer(nextOdd(windowSize)) {}

void SincFilter::push(sample_t in) {
  buffer.push(in);
}

sample_t SincFilter::get(double offset) const {
  assert(offset >= 0. && offset <= 1.);
  int32_t middle = buffer.size() / 2;
  sample_t ans = 0.;

  for(int32_t i = middle; i < static_cast<int32_t>(buffer.size()); i++) {
    ans += buffer.get(i) * lookup.get(i - middle, offset);
  }

  offset = 1. - offset;
  for(int32_t i = 0; i < middle; i++) {
    ans += buffer.get(i) * lookup.get(middle - i - 1, offset);
  }

  return ans;
}


// SampeRateConverter-----------------------------------------------------------------------

SampleRateConverter::SampleRateConverter(Frequency inSampleRate_p, Frequency outSampleRate_p, size_t filterLength) :
  inSampleRate(inSampleRate_p),
  outSampleRate(outSampleRate_p),
  diffrentSampleRates(inSampleRate != outSampleRate),
  step(inSampleRate_p / outSampleRate_p)
{
  if(diffrentSampleRates) {
    filter = SincFilter(inSampleRate, std::min(inSampleRate, outSampleRate) / 2., filterLength, WindowFunction::Type::Blackman);
  }
}

void SampleRateConverter::setOutSampleRateNoFilterUpdate(Frequency outSampleRate_p) {
  step = inSampleRate / outSampleRate_p;
}

void SampleRateConverter::push(sample_t in) {
  if(position >= 1.) {
    position -= 1.;
    out = in;
    if(diffrentSampleRates) {
      filter.push(in);
    }
  }
}

bool SampleRateConverter::outReady() {
  return position < 1.;
}

sample_t SampleRateConverter::get() {
  if(!outReady()) {
    return 0.;
  }
  if(diffrentSampleRates) {
    out = filter.get(position);
  }
  position += step;
  return out;
}


} // namespace ZAudio::Tools