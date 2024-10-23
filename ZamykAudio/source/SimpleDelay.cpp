#include <ZAudio/SimpleDelay.h>

namespace ZAudio::Tools {
  

SimpleDelay::SimpleDelay(Frequency sampleRate_p, Time delayTime, Time reservedDelayTime) : 
    sampleRate(sampleRate_p), 
    delayInSamples(sampleRate.Hz() * delayTime.seconds()),
    delay(std::ceil(sampleRate.Hz() * std::max(delayTime, reservedDelayTime).seconds() + 1)) {}  

sample_t SimpleDelay::get() const {    
  return delay.getFrictional(delayInSamples);
}

void SimpleDelay::push(sample_t in) {
  delay.push(in);
}

void SimpleDelay::setDelayTime(Time delayTime) {    
  delayInSamples = sampleRate.Hz() * delayTime.seconds();
  if(std::ceil(delayInSamples) > delay.size()) {
    delay.resize(std::ceil(delayInSamples) + 1);
  }
}

Time SimpleDelay::getDelayTime() const {
  return Time::seconds(delayInSamples / sampleRate.Hz());
}

double SimpleDelay::getDelayInSamples() const {  
  return delayInSamples;
}


} // namespace ZAudio::Tools