#pragma once

#include <ZAudio/Effect.h>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/CircularBuffer.h>

namespace ZAudio {


class LooperEffect : public Effect {
public:
  enum struct Mode {
    Paused,
    Recording,
    Overdubbing,
    Playing
  };
  struct Parameters {
    Time maxDuration = Time::seconds(1);
    Volume dry = Volume::dB(0);
    Volume wet = Volume::dB(0);

    Parameters() = default;
    Parameters(Time maxDuration_p, Volume dry_p, Volume wet_p);
  };

  enum : uint32_t {
    MaxDurationID,
    DryID,
    WetID,
    ModeID,   
    ClearID 
  };

  LooperEffect(Parameters parameters_p);

  FrameFormat getOutputFormat() const override {
    return FrameFormat::Mono;
  }

  FrameFormat getInputFormat() const override {
    return FrameFormat::Mono;
  }

  void process(std::span<const sample_t> in, std::span<sample_t> out) override;
  void setParameter(size_t id, ParameterValue value) override;
  void setSampleRate(Frequency sampleRate) override;
  uint32_t getTailTime() const override;

  std::unique_ptr<Effect> clone() const override;

  Result save(Tools::TreeDatabaseWriter writer) const override {
    assert(false && "Looper effect is not serializable");
    return Result::error("Looper effect is not serializable");
  }

  Result load(Tools::TreeDatabaseReader reader) override {
    assert(false && "Looper effect is not serializable");
    return Result::error("Looper effect is not serializable");
  }
  
  std::string getID() const override;
  int64_t getVersion() const override;
private:
  Parameters parameters;  
  Mode mode = Mode::Paused;

  bool filling = true;  
  uint32_t duration = 0;  
  uint32_t maxDuration = 0;  
  Tools::CircularBuffer<sample_t> buffer;
  Tools::CircularBuffer<sample_t>::Iterator curr;

  void clear();
};


} // name space