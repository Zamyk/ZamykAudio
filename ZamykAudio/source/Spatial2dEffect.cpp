#include <ZAudio/Spatial2dEffect.h>
#include <numbers>


namespace ZAudio::Tools {


Spatial2dEar::Spatial2dEar(Frequency sampleRate_p, Time maxDelay_p, Volume minGain_p, Volume maxFilteredPart_p, Frequency lowPassFrequency_p, double angle_p) :
  sampleRate(sampleRate_p),
  maxDelay(maxDelay_p),
  minGain(minGain_p),        
  maxFilteredPart(maxFilteredPart_p),
  angle(angle_p),
  delay(sampleRate, Time::seconds(0), maxDelay),  
  filter(AnalogFilter::Parameters::createLowPassParameters(sampleRate, lowPassFrequency_p)) 
  {
    update();
  }

sample_t Spatial2dEar::process(sample_t in) {
  delay.push(in);    
  sample_t out = delay.get();
  return (filter.process(out) * filterPart.linear() + out * (1. - filterPart.linear())) * gain.linear();
}

double Spatial2dEar::calculateDistance(double angle) {
  return std::fabs(sin(std::numbers::pi / 4. + angle / 2.)); // (90 + alpha) / 2.
}  

void Spatial2dEar::setMaxDelay(Time maxDelay_p) {
  maxDelay = maxDelay_p;
  update();
}

void Spatial2dEar::setMinGain(Volume minGain_p) {
  minGain = minGain_p;
  update();
}

void Spatial2dEar::setMaxFilteredPart(Volume maxFilteredPart_p) {
  maxFilteredPart = maxFilteredPart_p;
  update();
}

void Spatial2dEar::setAngle(double angle_p) {    
  angle = angle_p;
  update();  
}

void Spatial2dEar::setLowPassFrequency(Frequency frequency) {
  filter = AnalogFilter(AnalogFilter::Parameters::createLowPassParameters(sampleRate, frequency));
}

void Spatial2dEar::update() {
  double dist = calculateDistance(angle);      
  delay.setDelayTime(maxDelay * dist);                                             // the bigger distance the bigger delay                                
  filterPart = Volume::dB(NormalizedValue(dist).bind(-96, maxFilteredPart.dB()));  // the bigger distance the more filtering
  gain = Volume::dB(NormalizedValue(1. - dist).bind(minGain.dB(), 0.));         // the smaller distance the less volume    
}


} // namespace ZAudio::Tools



namespace ZAudio {


Spatial2dEffect::Spatial2dEffect(Parameters parameters_p) : parameters(parameters_p) {}
    

void Spatial2dEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = left.process(in[0]);
  out[1] = right.process(in[0]);    
}

void Spatial2dEffect::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case MaxEarDelayID:
      parameters.maxEarDelay = value.getTime();
      left.setMaxDelay(parameters.maxEarDelay);
      right.setMaxDelay(parameters.maxEarDelay);
      break;
    
    case MinEarGainID:
      parameters.minEarGain = value.getVolume();
      left.setMinGain(parameters.minEarGain);
      right.setMinGain(parameters.minEarGain);
      break;
    
    case MaxFilteredPartID:
      parameters.maxFilteredPart = value.getVolume();
      left.setMaxFilteredPart(parameters.maxFilteredPart);
      right.setMaxFilteredPart(parameters.maxFilteredPart);
      break;

    case LowPassFrequencyID:
      parameters.lowPassFrequency = value.getFrequency();
      left.setLowPassFrequency(parameters.lowPassFrequency);
      right.setLowPassFrequency(parameters.lowPassFrequency);
      break;

    case SoundAngleID:
      parameters.soundAngle = value.getNonInteger();
      left.setAngle(parameters.soundAngle);
      right.setAngle(parameters.soundAngle);
      break;
    
    default:
      assert(false);
  }
}
    
void Spatial2dEffect::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;    
  left = Tools::Spatial2dEar(sampleRate, parameters.maxEarDelay, parameters.minEarGain, parameters.maxFilteredPart, parameters.lowPassFrequency, parameters.soundAngle);
  right = Tools::Spatial2dEar(sampleRate, parameters.maxEarDelay, parameters.minEarGain, parameters.maxFilteredPart, parameters.lowPassFrequency, -parameters.soundAngle);    
}

std::unique_ptr<Effect> Spatial2dEffect::clone() const {
  return std::make_unique<Spatial2dEffect>(*this);
}

static std::string getName(uint32_t ind) {
  std::array<std::string, Spatial2dEffect::NumOfParameters> names = {
    "MaxEarDelay",
    "MinEarGain",
    "MaxFilteredPart",
    "LowPassFrequency",
    "SoundAngle"
  };
  return names[ind];
}

Result Spatial2dEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result r = writer.addValue(getName(MaxEarDelayID), parameters.maxEarDelay);
  r &= writer.addValue(getName(MinEarGainID), parameters.minEarGain);
  r &= writer.addValue(getName(MaxFilteredPartID), parameters.maxFilteredPart);
  r &= writer.addValue(getName(LowPassFrequencyID), parameters.lowPassFrequency);
  r &= writer.addValue(getName(SoundAngleID), parameters.soundAngle);
  return r;
}

Result Spatial2dEffect::load(Tools::TreeDatabaseReader reader) {
  Result r = reader.getValue(getName(MaxEarDelayID), parameters.maxEarDelay);
  r &= reader.getValue(getName(MinEarGainID), parameters.minEarGain);
  r &= reader.getValue(getName(MaxFilteredPartID), parameters.maxFilteredPart);
  r &= reader.getValue(getName(LowPassFrequencyID), parameters.lowPassFrequency);
  r &= reader.getValue(getName(SoundAngleID), parameters.soundAngle);
  return r;
}

std::string Spatial2dEffect::getID() const {
  return "ZA_Spatial2dEffect";
}

int64_t Spatial2dEffect::getVersion() const {
  return 1;  
}

uint32_t Spatial2dEffect::getTailTime() const {
  return parameters.maxEarDelay.seconds() * sampleRate.Hz();
}

Spatial2dEffect::Parameters Spatial2dEffect::getParameters() const {
  return parameters;
}


} // namespace ZAudio