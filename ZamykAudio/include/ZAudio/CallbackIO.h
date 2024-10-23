#pragma once

#include <fstream>
#include <thread>

#include <ZAudio/CommonTypes.h>
#include <ZAudio/AudioOutput.h>
#include <ZAudio/AudioInput.h>
#include <ZAudio/SampleRateConversion.h>

namespace ZAudio::Tools {

class CallbackInput; // forward
class CallbackOutput; // forward


class CallbackData {    
public:
  CallbackData() = default;  

  void init(uint32_t numberOfChannels_p, uint32_t bufferSize);
  
  template<typename T>
  void inputCallback(std::span<const T> in) {    
    if(!bufferEmpty) {      
      return;
    }
    assert(in.size() == buffer.size());    
    std::copy(in.begin(), in.end(), buffer.begin());    
    bufferEmpty = false;
  }

  template<typename T>
  void outputCallback(std::span<T> in) {    
    while(bufferEmpty && !ended) {            
      std::this_thread::yield();
    }
    if(ended) {
      return;
    }
    for(auto& v : in) {
      v = std::clamp(v, static_cast<T>(-0.999), static_cast<T>(0.999));
    }
    assert(in.size() == buffer.size());
    std::copy(buffer.cbegin(), buffer.cend(), in.begin());
    bufferEmpty = true;
  }

  size_t getNumberOfChannels() const;
  void setEnded();  

private:  
  std::atomic_bool ended{false};
  std::vector<sample_t> buffer;
  std::atomic_bool bufferEmpty;  
  uint32_t numberOfChannels = 0;
  std::atomic_bool error{false};
  std::string errorDescription;  
  friend CallbackInput;
  friend CallbackOutput;
};

struct InputOutputCallbackData {
  std::shared_ptr<CallbackData> input = std::make_shared<CallbackData>();
  std::shared_ptr<CallbackData> output = std::make_shared<CallbackData>();
};

class CallbackInput : public AudioInput {
public:
  CallbackInput(FrameFormat format_p, Frequency inSampleRate_p, std::shared_ptr<CallbackData> callbackData_p, bool blocking_p);

  void get(std::span<sample_t> out) override;
  void setSampleRate(Frequency sampleRate) override;
  void setParameter(size_t id, ParameterValue value) override;
  bool errorOccured() const override;
  bool isPlaying() const override;
  FrameFormat getFormat() const override;

  // only move constructors
  CallbackInput(const CallbackInput& oth) = delete;
  CallbackInput(CallbackInput&& oth) = default;
  CallbackInput& operator = (const CallbackInput& oth) = delete;
  CallbackInput& operator = (CallbackInput&& oth) = default;

  ~CallbackInput() { // TMPDBG
    std::ofstream out("callbackInputLog.txt");
    for(auto& v : log) {
      out << v << "\n";
    }
  }
private:
  FrameFormat format = FrameFormat::Mono;
  Frequency inSampleRate;
  std::shared_ptr<CallbackData> callbackData;
  bool blocking = false;

  std::vector<std::string> log; // TMPDBG
  
  size_t ind = 0;
  std::vector<sample_t> buffer;
  std::vector<Tools::SampleRateConverter> sampleRateConverters;
};

class CallbackOutput : public AudioOutput {
public:
  CallbackOutput(FrameFormat format_p, Frequency outSampleRate_p, std::shared_ptr<CallbackData> callbackData_p, bool blocking_p);

  void send(std::span<const sample_t> in) override;
  void setSampleRate(Frequency sampleRate) override;
  void setParameter(size_t id, ParameterValue value) override;
  bool errorOccured() const override;
  bool ended() const override;
  FrameFormat getFormat() const override;

  // only move constructors
  CallbackOutput(const CallbackOutput& oth) = delete;
  CallbackOutput(CallbackOutput&& oth) = default;
  CallbackOutput& operator = (const CallbackOutput& oth) = delete;
  CallbackOutput& operator = (CallbackOutput&& oth) = default;

  ~CallbackOutput() {
    std::ofstream out("callbackOutputLog.txt");
    for(auto& v : log) {
      out << v << "\n";
    }
  }
private:
  std::vector<std::string> log; // TMPDBG

  FrameFormat format;
  Frequency outSampleRate;
  std::shared_ptr<CallbackData> callbackData;    
  bool blocking = false;

  size_t ind = 0;
  std::vector<sample_t> buffer;
  std::vector<Tools::SampleRateConverter> sampleRateConverters;
};


} // namespace ZAudio::Tools