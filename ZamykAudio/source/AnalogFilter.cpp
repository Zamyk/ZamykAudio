#include <ZAudio/AnalogFilter.h>

#include <numbers>

namespace ZAudio::Tools {


AnalogFilter::Parameters::Parameters(Type type_p, Frequency sampleRate_p, Frequency frequency_p, double q_p, Volume gain_p) :
  type(type_p),
  sampleRate(sampleRate_p),
  frequency(frequency_p),  
  gain(gain_p),
  q(q_p) {}  

AnalogFilter::Parameters createByPassParameters() {
  return AnalogFilter::Parameters();
}

AnalogFilter::Parameters AnalogFilter::Parameters::createAllPassParameters(Frequency sampleRate, Frequency frequency) {
  return AnalogFilter::Parameters(Type::AllPass, sampleRate, frequency);
}

AnalogFilter::Parameters AnalogFilter::Parameters::createLowPassParameters(Frequency sampleRate, Frequency frequency, double q) {
  return AnalogFilter::Parameters(Type::LowPass, sampleRate, frequency, q);
}

AnalogFilter::Parameters AnalogFilter::Parameters::createHighPassfParameters(Frequency sampleRate, Frequency frequency, double q) {
  return AnalogFilter::Parameters(Type::HighPass, sampleRate, frequency, q);
}

AnalogFilter::Parameters AnalogFilter::Parameters::createFirstOrderLowPassParameters(Frequency sampleRate, Frequency frequency) {
  return AnalogFilter::Parameters(Type::FirstOrderLowPass, sampleRate, frequency);
}

AnalogFilter::Parameters AnalogFilter::Parameters::createFirstOrderHighPassfParameters(Frequency sampleRate, Frequency frequency) {
  return AnalogFilter::Parameters(Type::FirstOrderHighPass, sampleRate, frequency);
}

AnalogFilter::Parameters AnalogFilter::Parameters::createHighShelfParameters(Frequency sampleRate, Frequency frequency, Volume gain) {
  return AnalogFilter::Parameters(Type::HighShelf, sampleRate, frequency, 0, gain);
}
                                                   
AnalogFilter::Parameters AnalogFilter::Parameters::createLowShelfParameters(Frequency sampleRate, Frequency frequency, Volume gain) {
  return AnalogFilter::Parameters(Type::LowShelf, sampleRate, frequency, 0, gain);
}

AnalogFilter::Parameters AnalogFilter::Parameters::createBandPassParameters(Frequency sampleRate, Frequency frequency, double q) {
  return AnalogFilter::Parameters(Type::BandPass, sampleRate, frequency, q);
}

AnalogFilter::Parameters AnalogFilter::Parameters::createBandStopParameters(Frequency sampleRate, Frequency frequency, double q) {
  return AnalogFilter::Parameters(Type::BandStop, sampleRate, frequency, q);
}


AnalogFilter::AnalogFilter() : 
  aDelay(2),
  bDelay(2) {}

AnalogFilter::AnalogFilter(Parameters parameters) : aDelay(2), bDelay(2) {
  reset(parameters);
}    

void AnalogFilter::reset(Parameters parameters) {
  setByPass();
  switch(parameters.type) {
    case Type::ByPass:        
    break;

    case Type::AllPass:
      setAllPass(parameters.sampleRate, parameters.frequency);
    break;

    case Type::LowPass:
      setLowPass(parameters.sampleRate, parameters.frequency, parameters.q);
     break;

    case Type::HighPass:
      setHighPass(parameters.sampleRate, parameters.frequency, parameters.q);
    break;

    case Type::FirstOrderLowPass:
      setFirstOrderLowPass(parameters.sampleRate, parameters.frequency);
     break;

    case Type::FirstOrderHighPass:
      setFirstOrderHighPass(parameters.sampleRate, parameters.frequency);
    break;

    case Type::LowShelf:
      setLowShelf(parameters.sampleRate, parameters.frequency, parameters.gain);
    break;

    case Type::HighShelf:
      setHighShelf(parameters.sampleRate, parameters.frequency, parameters.gain);
    break;

    case Type::BandPass:
      setBandPass(parameters.sampleRate, parameters.frequency, parameters.q);
    break;

    case Type::BandStop:
      setBandStop(parameters.sampleRate, parameters.frequency, parameters.q);      
    break;

    default:
      assert(false);
    break;
  }
}

sample_t AnalogFilter::process(sample_t in) {          
  sample_t out = in * a0 + aDelay.get(0) * a1 + aDelay.get(1) * a2 - bDelay.get(0) * b1 - bDelay.get(1) * b2;
  aDelay.push(in);
  bDelay.push(out);    
  return out;
}

void AnalogFilter::setByPass() {
  a0 = 1;
  a1 = a2 = b1 = b2 = 0;
}

void AnalogFilter::setAllPass(Frequency sampleRate, Frequency frequency) {
  const double tmpTan = tan(std::numbers::pi * frequency.Hz() / sampleRate.Hz());
  const double alpha = ( tmpTan - 1 ) / ( tmpTan + 1 );
  a0 = alpha;
  a1 = 1.;
  b1 = alpha;    
}

void AnalogFilter::setFirstOrderLowPass(Frequency sampleRate, Frequency frequency) {
  const double Nyquist = sampleRate.Hz();
  const double f = 2 * std::numbers::pi * frequency.Hz() / Nyquist;
  const double gamma = cos(f) / (1 + sin(f));
  a0 = (1 - gamma) / 2.;
  a1 = a0;
  b1 = -gamma;      
}

void AnalogFilter::setFirstOrderHighPass(Frequency sampleRate, Frequency frequency) {
  const double Nyquist = sampleRate.Hz();
  const double f = 2 * std::numbers::pi * frequency.Hz() / Nyquist;
  const double gamma = cos(f) / (1 + sin(f));
  a0 = (1 + gamma) / 2.;
  a1 = -a0;
  b1 = -gamma;    
}

void AnalogFilter::setLowPass(Frequency sampleRate, Frequency frequency, double q) {
  const double Nyquist = sampleRate.Hz();
  const double f = 2 * std::numbers::pi * frequency.Hz() / Nyquist;
  const double d = 1. / q;
  const double beta = 0.5 * (1. - (d / 2.) * sin(f)) / (1 + (d / 2.) * sin(f));
  const double gamma = (0.5 + beta) * cos(f);
  a0 = (0.5 + beta - gamma) / 2.;
  a1 = 2. * a0;
  a2 = a0;
  b1 = -2. * gamma;
  b2 = 2 * beta;  
}

void AnalogFilter::setHighPass(Frequency sampleRate, Frequency frequency, double q) {
  const double Nyquist = sampleRate.Hz();
  const double f = 2 * std::numbers::pi * frequency.Hz() / Nyquist;
  const double d = 1. / q;
  const double beta = 0.5 * (1. - (d / 2.) * sin(f)) / (1 + (d / 2.) * sin(f));
  const double gamma = (0.5 + beta) * cos(f);
  a0 = (0.5 + beta + gamma) / 2.;
  a1 = -2. * a0;
  a2 = a0;
  b1 = -2. * gamma;
  b2 = 2 * beta;
}

void AnalogFilter::setLowShelf(Frequency sampleRate, Frequency frequency, Volume gain) {
  const double K = tan(std::numbers::pi * frequency.Hz() / sampleRate.Hz());    
  const double V = Volume::dB(std::fabs(gain.dB())).linear();    
  if (gain.dB() >= 0) {    // boost
    const double norm = 1 / (1 + sqrt(2) * K + K * K);
    a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
    a1 = 2 * (V * K * K - 1) * norm;
    a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
    b1 = 2 * (K * K - 1) * norm;
    b2 = (1 - sqrt(2) * K + K * K) * norm;
  }
  else {    // cut
    const double norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
    a0 = (1 + sqrt(2) * K + K * K) * norm;
    a1 = 2 * (K * K - 1) * norm;
    a2 = (1 - sqrt(2) * K + K * K) * norm;
    b1 = 2 * (V * K * K - 1) * norm;
    b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;      
  }
}

void AnalogFilter::setHighShelf(Frequency sampleRate, Frequency frequency, Volume gain) {        
  const double K = tan(std::numbers::pi * frequency.Hz() / sampleRate.Hz());    
  const double V = Volume::dB(std::fabs(gain.dB())).linear();    
  if (gain.dB() >= 0) {    // boost
    const double norm = 1 / (1 + sqrt(2) * K + K * K);
    a0 = (V + sqrt(2*V) * K + K * K) * norm;
    a1 = 2 * (K * K - V) * norm;
    a2 = (V - sqrt(2*V) * K + K * K) * norm;
    b1 = 2 * (K * K - 1) * norm;
    b2 = (1 - sqrt(2) * K + K * K) * norm;      
  }
  else {    // cut
    const double norm = 1 / (V + sqrt(2*V) * K + K * K);
    a0 = (1 + sqrt(2) * K + K * K) * norm;
    a1 = 2 * (K * K - 1) * norm;
    a2 = (1 - sqrt(2) * K + K * K) * norm;
    b1 = 2 * (K * K - V) * norm;
    b2 = (V - sqrt(2*V) * K + K * K) * norm;      
  }    
}

void AnalogFilter::setBandPass(Frequency sampleRate, Frequency frequency, double q) {
  const double Nyquist = sampleRate.Hz();
  const double k = tan( (std::numbers::pi * frequency.Hz() / Nyquist ) );
  const double delta = k * k * q + k + q;
  a0 = k / delta;
  a1 = 0.;
  a2 = -k / delta;
  b1 = ( 2 * q * (k * k - 1) ) / delta;
  b2 = ( k * k * q - k + q ) / delta;    
}

void AnalogFilter::setBandStop(Frequency sampleRate, Frequency frequency, double q) {
  const double Nyquist = sampleRate.Hz();
  const double k = tan( (std::numbers::pi * frequency.Hz() / Nyquist ) );
  const double delta = k * k * q + k + q;
  a0 = ( q * (k * k + 1) ) / delta;
  a1 = ( 2 * q * (k * k - 1) ) / delta;
  a2 = a0;
  b1 = a1;
  b2 = (k * k * q - k + q) / delta;    
}

} // namespace ZAudio::Tools