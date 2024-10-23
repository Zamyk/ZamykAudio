#include <ZAudio/TubePreampEffect.h>

namespace ZAudio::Tools {


// TriodeClassA ---------------------------------------------------------------

TriodeClassA::TriodeClassA(Frequency sampleRate, Parameters parameters_p) : 
  parameters(parameters_p),
  highPassFilter(AnalogFilter::Parameters::createHighPassfParameters(sampleRate, parameters.highPassFrequency)),
  lowShelfFilter(AnalogFilter::Parameters::createLowShelfParameters(sampleRate, parameters.lowShelfFrequency, parameters.lowShelfGain)) {}  

sample_t TriodeClassA::process(sample_t in) {
  sample_t out = WaveShapers::process(parameters.waveShaper, in, parameters.saturation);
  if(parameters.invertOutput) {
    out *= -1.;
  }
  if(parameters.highPassOn) {
    out = highPassFilter.process(out);
  }
  if(parameters.lowShelfOn) {
    out = lowShelfFilter.process(out);
  }
  out *= parameters.gain.linear();    
  return out;
}


} // namespace ZAudio::Tools

namespace ZAudio {


// TubePreampEffect -------------------------------------------------------------------

TubePreampEffect::TubePreampEffect(Parameters parameters_p) : parameters(parameters_p) {}

FrameFormat TubePreampEffect::getOutputFormat() const {
  return FrameFormat::Mono;
}

FrameFormat TubePreampEffect::getInputFormat() const {
  return FrameFormat::Mono;
}

void TubePreampEffect::process(std::span<const sample_t> in, std::span<sample_t> out) {  
  overSampler.push(in[0]);    
  while(overSampler.outReady()) {
    overSampleReverter.push(preamp.process(overSampler.get()));                
  }      
  out[0] = overSampleReverter.get();    
}

void TubePreampEffect::setParameter(size_t id, ParameterValue value) {}

void TubePreampEffect::setSampleRate(Frequency sampleRate) {    
  Preamp::Parameters params;    

  params.highShelfFrequency = Frequency::Hz((parameters.frequencyRangeMax.Hz() - parameters.frequencyRangeMin.Hz()) * (3. / 4.) + parameters.frequencyRangeMin.Hz());
  params.highShelfGain = parameters.high;

  params.inputGain = parameters.inputGain;

  params.lowShelfFrequency = Frequency::Hz((parameters.frequencyRangeMax.Hz() - parameters.frequencyRangeMin.Hz()) / 4. + parameters.frequencyRangeMin.Hz());
  params.lowShelfGain = parameters.low;

  params.outputGain = parameters.outputGain;

  params.triodeLowShelfFrequency = Frequency::Hz(parameters.frequencyRangeMin.Hz() + 10);
  params.triodeLowShelfGain = Volume::dB(-6);

  params.waveShaper = Tools::WaveShapers::WaveShaperType::ArcTangent;
  preamp = Preamp(sampleRate * overSampleRatio, params);

  overSampler = Tools::SampleRateConverter(sampleRate, sampleRate * overSampleRatio);
  overSampleReverter = Tools::SampleRateConverter(sampleRate * overSampleRatio, sampleRate);
}

uint32_t TubePreampEffect::getTailTime() const {
  return 0;
}

std::unique_ptr<Effect> TubePreampEffect::clone() const {
  return std::make_unique<TubePreampEffect>(*this) ;
}

static std::string getName(uint32_t v) {
  const std::array<std::string, TubePreampEffect::NumOfParameters> names = {
    "InputGain",
    "OutputGain",
    "FrequencyRangeMin",
    "FrequencyRangeMax",
    "Low",
    "High"
  };
  return names[v];
}

Result TubePreampEffect::save(Tools::TreeDatabaseWriter writer) const {
  Result res = Result::success();
  res &= writer.addValue(getName(InputGainID), parameters.inputGain);
  res &= writer.addValue(getName(OutputGainID), parameters.outputGain);
  res &= writer.addValue(getName(FrequencyRangeMinID), parameters.frequencyRangeMin);
  res &= writer.addValue(getName(FrequencyRangeMaxID), parameters.frequencyRangeMax);
  res &= writer.addValue(getName(LowID), parameters.low);
  res &= writer.addValue(getName(HighID), parameters.high);
  return res;
}

Result TubePreampEffect::load(Tools::TreeDatabaseReader reader) {
  Result res = Result::success();
  res &= reader.getValue(getName(InputGainID), parameters.inputGain);
  res &= reader.getValue(getName(OutputGainID), parameters.outputGain);
  res &= reader.getValue(getName(FrequencyRangeMinID), parameters.frequencyRangeMin);
  res &= reader.getValue(getName(FrequencyRangeMaxID), parameters.frequencyRangeMax);
  res &= reader.getValue(getName(LowID), parameters.low);
  res &= reader.getValue(getName(HighID), parameters.high);
  return res;
}

std::string TubePreampEffect::getID() const {
  return "ZA_TubePreampEffect";
}

int64_t TubePreampEffect::getVersion() const {
  return 1;
}

TubePreampEffect::Parameters TubePreampEffect::getParameters() const {
  return parameters;
}

} // namespace ZAudio