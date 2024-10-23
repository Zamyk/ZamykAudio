#include <ZAudio/AudioDelay.h>

namespace ZAudio::Tools {
  
  
AudioDelay::AudioDelay(Frequency sampleRate_p, Time delayTime_p, Time reservedDelayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p) : 
  sampleRate(sampleRate_p),   
  delay(sampleRate_p, delayTime_p, reservedDelayTime_p),
  dry(dry_p), wet(wet_p),
  feedback(feedback_p) {}  

  
void AudioDelay::setDry(Volume dry_p) {
  dry = dry_p;
}

void AudioDelay::setWet(Volume wet_p) {
  wet = wet_p;
}

void AudioDelay::setFeedback(Volume feedback_p) {
  feedback = feedback_p;
}

void AudioDelay::setDelayTime(Time delayTime) {  
  delay.setDelayTime(delayTime);  
}  

sample_t AudioDelay::process(sample_t in) {  
  sample_t y = delay.get();           
  delay.push(feedback.linear() * y + in);    
  return wet.linear() * y + dry.linear() * in;
}

double AudioDelay::getDelayTimeInSamples() const {
  return delay.getDelayInSamples();
}

double AudioDelay::getRT60TimeInSamples() const {
  return calculateRT60Time(delay.getDelayTime(), feedback).seconds() * sampleRate.Hz();
}

Time AudioDelay::calculateRT60Time(Time delayTime, Volume feedback) {
  if(feedback.dB() >= 0.) {    
    return delayTime;
  }
  return delayTime * (60. / -feedback.dB());
}


} // namespace ZAudio::Tools