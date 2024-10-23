#pragma once

#include <ZAudio/CommonTypes.h>
#include <ZAudio/Effect.h>
#include <ZAudio/WaveShapers.h>
#include <ZAudio/AnalogFilter.h>
#include <ZAudio/SampleRateConversion.h>

namespace ZAudio::Tools {


class TriodeClassA {
public:
struct Parameters {  
  WaveShapers::WaveShaperType waveShaper = WaveShapers::WaveShaperType::Cube;
  double saturation = 1.;
  Volume gain = Volume::linear(1.);
  bool invertOutput = true;    
      
  bool highPassOn = true;
  Frequency highPassFrequency = Frequency::Hz(30);
  
  bool lowShelfOn = false;    
  Frequency lowShelfFrequency = Frequency::Hz(30);
  Volume lowShelfGain;
};
  TriodeClassA() = default;
  TriodeClassA(Frequency sampleRate, Parameters parameters_p);

  sample_t process(sample_t in);

private:
  Parameters parameters;  
  AnalogFilter highPassFilter;
  AnalogFilter lowShelfFilter;
};

template<size_t NumberOfTriodes, size_t FilterPosition>
class TubePreampClassA {
public:
struct Parameters {
  Volume inputGain;
  Volume outputGain;
  
  Frequency triodeLowShelfFrequency;
  Volume triodeLowShelfGain;

  Frequency lowShelfFrequency;
  Volume lowShelfGain;
  Frequency highShelfFrequency;
  Volume highShelfGain;
  
  WaveShapers::WaveShaperType waveShaper;
};
  TubePreampClassA() = default;

  TubePreampClassA(Frequency sampleRate, Parameters parameters_p) : 
    parameters(parameters_p),     
    lowShelfFilter(AnalogFilter::Parameters::createLowShelfParameters(sampleRate, parameters.lowShelfFrequency, parameters.lowShelfGain)),
    highShelfFilter(AnalogFilter::Parameters::createHighShelfParameters(sampleRate, parameters.highShelfFrequency, parameters.highShelfGain)) {

      std::fill(triodes.begin(), triodes.end(), TriodeClassA(sampleRate, getTriodeParameters()));
    }

  sample_t process(sample_t in) {
    sample_t out = in * parameters.inputGain.linear();
    for(size_t i = 0; i < FilterPosition; i++) {
      out = triodes[i].process(out);
    }

    out = lowShelfFilter.process(out);
    out = highShelfFilter.process(out);
    
    for(size_t i = FilterPosition; i < triodes.size(); i++) {
      out = triodes[i].process(out);
    }

    return out * parameters.outputGain.linear();
  }

private:
  Parameters parameters;
  std::array<TriodeClassA, NumberOfTriodes> triodes;
  AnalogFilter lowShelfFilter;
  AnalogFilter highShelfFilter;

  TriodeClassA::Parameters getTriodeParameters() {
    TriodeClassA::Parameters ans;    
    ans.invertOutput = true;
    ans.highPassOn = true;
    ans.highPassFrequency = Frequency::Hz(5);
    ans.gain = Volume::dB(0.);
    ans.saturation = 1.;
    ans.lowShelfOn = true;    
    ans.lowShelfFrequency = parameters.triodeLowShelfFrequency;
    ans.lowShelfGain = parameters.lowShelfGain;
    ans.waveShaper = parameters.waveShaper;
    return ans;
  }
};

} // namespace

namespace ZAudio {


class TubePreampEffect : public Effect {
  using Preamp = Tools::TubePreampClassA<4, 2>;
public:  
struct Parameters {  
  Volume inputGain;
  Volume outputGain;
  Frequency frequencyRangeMin;
  Frequency frequencyRangeMax;
  Volume low;
  Volume high;
};
  static constexpr uint32_t NumOfParameters = 6;
  enum : uint32_t {
    InputGainID,
    OutputGainID,
    FrequencyRangeMinID,
    FrequencyRangeMaxID,
    LowID,
    HighID
  };

  TubePreampEffect(Parameters parameters_p);

  FrameFormat getOutputFormat() const override;
  FrameFormat getInputFormat() const override;

  void process(std::span<const sample_t> in, std::span<sample_t> out) override;
  void setParameter(size_t id, ParameterValue value) override;
  void setSampleRate(Frequency sampleRate) override;
  uint32_t getTailTime() const override;

  std::unique_ptr<Effect> clone() const override;
  Result save(Tools::TreeDatabaseWriter writer) const override;
  Result load(Tools::TreeDatabaseReader reader) override;
  std::string getID() const override;
  int64_t getVersion() const override;
  Parameters getParameters() const;
private:
  Parameters parameters;
  Preamp preamp;    
  static constexpr double overSampleRatio = 4.;
  Tools::SampleRateConverter overSampler;
  Tools::SampleRateConverter overSampleReverter;
};



} // namespace
