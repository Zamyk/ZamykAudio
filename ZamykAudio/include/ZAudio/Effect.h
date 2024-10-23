#pragma once

#include <string>
#include <span>

#include <ZAudio/CommonTypes.h>
#include <ZAudio/FrameFormat.h>
#include <ZAudio/TreeDatabase.h>
#include <ZAudio/EffectSerializer.h>
#include <ZAudio/SoundBuffer.h>

namespace ZAudio {

class Effect {
public:
  virtual ~Effect() = default;
  virtual FrameFormat getOutputFormat() const = 0;
  virtual FrameFormat getInputFormat() const = 0;
  virtual void process(std::span<const sample_t> in, std::span<sample_t> out) = 0;    
  virtual void setParameter(size_t id, ParameterValue value) = 0;
  virtual void setParameter(size_t id1, size_t id2, ParameterValue value) {}
  virtual ParameterValue getOutputValue(size_t id) { return ParameterValue(); }  
  virtual void setSampleRate(Frequency sampleRate) = 0;
  
  virtual std::unique_ptr<Effect> clone() const = 0;
  virtual Result save(Tools::TreeDatabaseWriter writer) const = 0;
  virtual Result load(Tools::TreeDatabaseReader reader) = 0;
  virtual std::string getID() const = 0;
  virtual int64_t getVersion() const = 0;
  virtual uint32_t getTailTime() const = 0;  
};

template<typename Out>
static Result saveEffectToXML(const Effect& effect, Out& outStream) {      
  Tools::TreeDatabase database;
  if(auto res = Tools::EffectSerializer::instance().save(database, effect); !res) {
    return res;
  }
  return database.toXml(outStream);   
}


template<typename In>
static ResultValue<std::unique_ptr<Effect>> loadEffectFromXML(In& inStream) {  
  Tools::TreeDatabase database;
  if(auto result = database.fromXML(inStream); !result) {
    return result;
  }
  return Tools::EffectSerializer::instance().load(database);
}

inline SoundBuffer processBuffer(Effect& effect, const SoundBuffer& input) {  
  effect.setSampleRate(input.getSampleRate());
  SoundBuffer output(input.getSampleRate(), effect.getOutputFormat(), input.getLength() + effect.getTailTime());

  std::array<sample_t, Tools::MaxNumberOfChannels> frame1;
  std::array<sample_t, Tools::MaxNumberOfChannels> frame2;
  std::fill(frame1.begin(), frame1.end(), 0.);
  std::fill(frame2.begin(), frame2.end(), 0.);

  for(size_t i = 0; i < output.getLength(); i++) {
    if(i < input.getLength()) {
     input.getFrame(i, frame1.begin());
    }
    else {
      std::fill(frame1.begin(), frame1.end(), 0.);
    }

    Tools::convertFrames(frame1, input.getFrameFormat(), frame2, effect.getInputFormat());    

    effect.process(frame2, frame1);

    output.setFrame(i, frame1.begin());
  }

  return output;
}


} // namespace ZAudio