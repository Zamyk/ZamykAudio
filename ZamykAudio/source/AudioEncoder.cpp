#include <ZAudio/AudioEncoder.h>

namespace ZAudio {


Result encodeSound(AudioEncoder& encoder, const SoundBuffer& soundBuffer) {  
  assert(encoder.getSampleRate() == soundBuffer.getSampleRate());
  assert(encoder.getFormat() == soundBuffer.getFrameFormat());    
  std::array<sample_t, Tools::MaxNumberOfChannels> frame;
  for(size_t i = 0; i < soundBuffer.getLength(); i++) {
    soundBuffer.getFrame(i, frame.begin());    
    encoder.send(frame);    
  }
  if(encoder.errorOccured()) {
    return Result::error("error encoding sound");
  }
  return Result::success();
}


AsyncEncoder::AsyncEncoder(std::unique_ptr<AudioEncoder> encoder_p, Time bufferedTime) : 
  encoder(std::move(encoder_p)), 
  buffer(bufferedTime.seconds() * encoder->getSampleRate().Hz() + 1), 
  sampleRate(encoder->getSampleRate()),
  format(encoder->getFormat()),
  thread(&AsyncEncoder::asyncThread, this)
{
  run = true;
  ready = true;  
}

AsyncEncoder::~AsyncEncoder() {
  run = false;
  thread.join();
}

void AsyncEncoder::send(std::span<const sample_t> out) {
  for(size_t i = 0; i < Tools::numberOfChannels(format); i++) {
    while(true) {
      if(buffer.tryPush(out[i])) {
        break;
      } 
      if(error || ended_) {
        break;
      }
      std::this_thread::yield();
    }
  }
}

Frequency AsyncEncoder::getSampleRate() const {
  return sampleRate;
}

FrameFormat AsyncEncoder::getFormat() const {
  return format;
}

bool AsyncEncoder::errorOccured() const {
  return error;
}

bool AsyncEncoder::ended() const {
  return ended_;
}

void AsyncEncoder::asyncThread() {
  while(!ready) {
    std::this_thread::yield();
  }
  std::vector<sample_t> frame(Tools::numberOfChannels(format));
  size_t last = 0;  
  while(run) {
        
    while(last < frame.size()) {
      while(true) {
        if(auto tmp = buffer.tryPop(); tmp) {
          frame[last++] = *tmp;
          break;
        }
        if(!run) {
          return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //std::this_thread::yield();
      }
    }
    if(last == frame.size()) {
      encoder->send(frame);
      last = 0;
    }
    if(encoder->ended()) {
      ended_ = true;
    }
    if(encoder->errorOccured()) {
      error = true;
    }
    std::this_thread::yield();
  }
}


FileOutput::FileOutput(std::unique_ptr<AudioEncoder> encoder_p) : encoder(std::move(encoder_p)) {}  

void FileOutput::send(std::span<const sample_t> in) {
  if(stop) {
    return;
  }
  for(size_t i = 0; i < sampleRateConverters.size(); i++) {
    sampleRateConverters[i].push(in[i]);
  }  
  while(sampleRateConverters.front().outReady()) {
    std::array<sample_t, Tools::MaxNumberOfChannels> frame;
    for(size_t i = 0; i < sampleRateConverters.size(); i++) {
      frame[i] = sampleRateConverters[i].get();
    }
    encoder->send(frame);
  }
}

void FileOutput::setSampleRate(Frequency sampleRate) {
  sampleRateConverters.resize(Tools::numberOfChannels(encoder->getFormat()), Tools::SampleRateConverter(sampleRate, encoder->getSampleRate()));
}

void FileOutput::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case StopID:
      stop = true;
      break;
    
    default:
      assert(false);      
  }
}

ParameterValue FileOutput::getOutputValue(size_t id) {
  assert(false);
  return ParameterValue();
}

bool FileOutput::errorOccured() const {
  return encoder->errorOccured();
}

bool FileOutput::ended() const {
  return stop || encoder->errorOccured();
}

FrameFormat FileOutput::getFormat() const {
  return encoder->getFormat();
}


} // namespace ZAudio