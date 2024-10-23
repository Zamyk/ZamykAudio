#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/SimpleDelay.h>

namespace ZAudio::Tools {


class AudioDelay {
public:
  AudioDelay() = default;    
  AudioDelay(Frequency sampleRate_p, Time delayTime_p, Time reservedDelayTime_p, Volume dry_p, Volume wet_p, Volume feedback_p);
  void setDry(Volume dry_p);
  void setWet(Volume wet_p);
  void setFeedback(Volume feedback_p);  
  void setDelayTime(Time delayTime);
  sample_t process(sample_t in);
  double getDelayTimeInSamples() const;
  double getRT60TimeInSamples() const;
  static Time calculateRT60Time(Time delayTime, Volume feedback);
private:  
  Frequency sampleRate;  
  SimpleDelay delay;
  Volume dry;
  Volume wet;
  Volume feedback;    
};


} // namespace ZAudio::Tools