#pragma once

#include <ZAudio/CommonTypes.h>

namespace ZAudio::Tools {


class AudioDetector {
public:
enum struct DetectMode {
  Peak, MeanSquare, RootMeanSquare
};  

  AudioDetector() = default;
  AudioDetector(Frequency sampleRate_p, DetectMode detectMode_p, Time attackTime_p, Time releaseTime_p, bool clamp_p);  
  Volume process(sample_t in);        
  void setDetectMode(DetectMode detectMode_p);
  void setAttackTime(Time attackTime_p);
  void setReleaseTime(Time releaseTime_p);
  void setClamp(bool clamp_p);

private:
  Frequency sampleRate;
  DetectMode detectMode = DetectMode::Peak;
  Time attackTime;
  Time releaseTime;
  bool clamp = true;  
  double attack = 0.;
  double release = 0.;
  double lastEnvelope = 0.;    
};


} // namespace ZAudio::Tools