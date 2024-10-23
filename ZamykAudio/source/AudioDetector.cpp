#include <ZAudio/AudioDetector.h>

using namespace ZAudio;
using namespace Tools;

namespace {

double convertAnalogParameter(Frequency sampleRate, Time time) {        
  constexpr double AnalogConverter = -0.99967234081320612357829304641019; // ln(36.7%)
  return exp(AnalogConverter / (time.seconds() * sampleRate.Hz()));
}  

} // namespace anonymous

ZAudio::Tools::AudioDetector::AudioDetector(Frequency sampleRate_p, DetectMode detectMode_p, Time attackTime_p, Time releaseTime_p, bool clamp_p) :
  sampleRate(sampleRate_p),
  detectMode(detectMode_p),
  attackTime(attackTime_p),
  releaseTime(releaseTime_p),
  clamp(clamp_p), 
  attack(convertAnalogParameter(sampleRate, attackTime)),
  release(convertAnalogParameter(sampleRate, releaseTime)) {}  


void AudioDetector::setDetectMode(DetectMode detectMode_p) {
  detectMode = detectMode_p;
}

void AudioDetector::setAttackTime(Time attackTime_p) {
  attackTime = attackTime_p;
  attack = convertAnalogParameter(sampleRate, attackTime);
}

void AudioDetector::setReleaseTime(Time releaseTime_p) {
  releaseTime = releaseTime_p;
  release = convertAnalogParameter(sampleRate, releaseTime);
}

void AudioDetector::setClamp(bool clamp_p) {
  clamp = clamp_p;
}

Volume AudioDetector::process(sample_t in) {
  in = std::fabs(in);
  if(detectMode == DetectMode::MeanSquare || (detectMode == DetectMode::RootMeanSquare)) {
    in *= in;
  }

  double currEnvelope = 0.;

  if(in > lastEnvelope) {
    currEnvelope = attack * (lastEnvelope - in) + in;
  }
  else {
    currEnvelope = release * (lastEnvelope - in) + in;
   }

  if(clamp) {
    currEnvelope = fmin(currEnvelope, 1.);
  }
  currEnvelope = fmax(currEnvelope, 0.);

  lastEnvelope = currEnvelope;

  if(detectMode == DetectMode::RootMeanSquare) {
    currEnvelope = std::sqrt(currEnvelope);
  }

  return Volume::linear(currEnvelope);
}