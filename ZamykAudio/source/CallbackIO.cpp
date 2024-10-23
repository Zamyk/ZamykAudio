#include <ZAudio/CallbackIO.h>

namespace ZAudio::Tools {


// CallbackData ----------------------------------------------------------------
     
void CallbackData::init(uint32_t numberOfChannels_p, uint32_t bufferSize) {
  numberOfChannels = numberOfChannels_p;
  buffer.resize(bufferSize);    
  bufferEmpty = true;
}
  
size_t CallbackData::getNumberOfChannels() const {
  return numberOfChannels;
}  

void CallbackData::setEnded() {
  ended = true;
}

// CallbacInput ----------------------------------------------------------------

CallbackInput::CallbackInput(FrameFormat format_p, Frequency inSampleRate_p, std::shared_ptr<CallbackData> callbackData_p, bool blocking_p) : 
  format(format_p),
  inSampleRate(inSampleRate_p),
  callbackData(callbackData_p),
  blocking(blocking_p),
  buffer(callbackData->buffer.size()) {}

void CallbackInput::get(std::span<sample_t> out) {      
  if(callbackData->ended) {
    return;
  }
  if(ind == buffer.size()) {
    if(blocking) {
      while(callbackData->bufferEmpty && !callbackData->ended) {        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
      if(callbackData->ended) {
        return;
      }   
    }
    else {
      if(callbackData->bufferEmpty) {          
        return;
      }
    }
    std::copy(callbackData->buffer.cbegin(), callbackData->buffer.cend(), buffer.begin());
    callbackData->bufferEmpty = true;
    ind = 0;
  }
    
  while(!sampleRateConverters.front().outReady()) {      
    if(ind == buffer.size()) {
      break;
    }
    for(size_t i = 0; i < sampleRateConverters.size(); i++) {
      sampleRateConverters[i].push(buffer[ind]);
      ind++;
    }
  }

  for(size_t i = 0; i < sampleRateConverters.size(); i++) {
    out[i] = sampleRateConverters[i].get();
  }
}

void CallbackInput::setSampleRate(Frequency sampleRate) {
  sampleRateConverters.resize(Tools::numberOfChannels(format));
  for(auto& converter : sampleRateConverters) {
    converter = Tools::SampleRateConverter(inSampleRate, sampleRate);
  }
}

void CallbackInput::setParameter(size_t id, ParameterValue value) {}

bool CallbackInput::errorOccured() const {
  return callbackData->error;
}

bool CallbackInput::isPlaying() const {
  return true;
}

FrameFormat CallbackInput::getFormat() const {
  return format;
}

// CallbackOutput -------------------------------------------------------------------------

CallbackOutput::CallbackOutput(FrameFormat format_p, Frequency outSampleRate_p, std::shared_ptr<CallbackData> callbackData_p, bool blocking_p) :
  format(format_p),
  outSampleRate(outSampleRate_p),
  callbackData(callbackData_p),
  blocking(blocking_p),
  buffer(callbackData->buffer.size()) {}

void CallbackOutput::send(std::span<const sample_t> in) {  
  if(callbackData->ended) {
    return;
  }
  for(size_t i = 0; i < sampleRateConverters.size(); i++) {
    sampleRateConverters[i].push(in[i]);
  }

  while(sampleRateConverters[0].outReady()) {                  
    if(ind == buffer.size()) {
      if(blocking) {
        while(!callbackData->bufferEmpty && !callbackData->ended) {          
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }          
        if(callbackData->ended) {
          return;
        }
      }
      else {
        if(!callbackData->bufferEmpty) {
          return;
        }
      }
      std::copy(buffer.cbegin(), buffer.cend(), callbackData->buffer.begin());
      ind = 0;
      callbackData->bufferEmpty = false;
    }

    for(size_t i = 0; i < sampleRateConverters.size(); i++) {
      buffer[ind] = sampleRateConverters[i].get();
      ind++;
    }
  }    
}

void CallbackOutput::setSampleRate(Frequency sampleRate) {
  sampleRateConverters.resize(Tools::numberOfChannels(format));
  for(auto& converter : sampleRateConverters) {
    converter = Tools::SampleRateConverter(sampleRate, outSampleRate);
  }
}

void CallbackOutput::setParameter(size_t id, ParameterValue value) {}

bool CallbackOutput::errorOccured() const {
  return callbackData->error;
}

bool CallbackOutput::ended() const {
  return false;
}

FrameFormat CallbackOutput::getFormat() const {
  return format;
}


} // namespace ZAudio