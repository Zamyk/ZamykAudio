#include <ZAudio/ModulatedDelay.h>

namespace ZAudio::Tools {


ModulatedDelay::ModulatedDelay(Frequency sampleRate_p, Parameters params) :
  sampleRate(sampleRate_p),
  parameters(params),
  lfo(sampleRate, params.rate, params.shape, params.phaseOffset),
  delay(sampleRate, params.minDelay, params.reservedDelay, params.dry, params.wet, params.feedback) {}

void ModulatedDelay::setRate(Frequency rate) {
  lfo.setFrequency(rate);
}

void ModulatedDelay::setMinDelay(Time time) {
  parameters.minDelay = time;
}

void ModulatedDelay::setMaxDelay(Time time) {
  parameters.maxDelay = time;
}

void ModulatedDelay::setWet(Volume wet) {
  delay.setWet(wet);
}

void ModulatedDelay::setDry(Volume dry) {
  delay.setDry(dry);
}

void ModulatedDelay::setFeedback(Volume feedback) {
  delay.setFeedback(feedback);
}

void ModulatedDelay::setShape(LowFrequencyOscillator::ShapeType shape) {
  lfo.setShape(shape);
}

void ModulatedDelay::setDepth(double depth_p) {
  parameters.depth = depth_p;
}
  
sample_t ModulatedDelay::process(sample_t in) {
  Time delayTime = Time::seconds(lfo.get().bindFromLeft(parameters.minDelay.seconds(), parameters.maxDelay.seconds(), parameters.depth));
  delay.setDelayTime(delayTime);
  return delay.process(in);
}  

uint32_t ModulatedDelay::getTailTime() const {    
  return AudioDelay::calculateRT60Time(parameters.maxDelay, parameters.feedback).seconds() * sampleRate.Hz();
}


} // namespace ZAudio::Tools