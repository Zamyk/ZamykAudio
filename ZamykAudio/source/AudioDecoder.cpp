#include <ZAudio/AudioDecoder.h>


namespace ZAudio {


AsyncDecoder::AsyncDecoder(std::unique_ptr<AudioDecoder> decoder_p, Time bufferedTime, bool looped_p) :
  decoder(std::move(decoder_p)),
  buffer(bufferedTime.seconds() * decoder->getSampleRate().Hz() + 1),
  length(decoder->getLength()),
  sampleRate(decoder->getSampleRate()),
  format(decoder->getFormat()),
  loopStart(decoder->getLoopStart()),
  loopEnd(decoder->getLoopEnd()),
  looped(looped_p),
  thread(&AsyncDecoder::asyncThread, this)
{
  run = true;
  ready = true;
}

AsyncDecoder::~AsyncDecoder() {
  run = false;
  thread.join();
}

bool AsyncDecoder::get(std::span<sample_t> out) {
  if(error) {
    return false;
  }
  for(size_t i = 0; i < Tools::numberOfChannels(format); i++) {
    auto v = buffer.tryPop();
    if(v) {
      out[i] = *v;
    }
    else {
      if(ended) {
        return false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  return true;
}

void AsyncDecoder::seek(uint64_t position_p) {
  seekPosition = position_p;
  askSeek = true;
}

uint64_t AsyncDecoder::getLength() {
  return length;
}

Frequency AsyncDecoder::getSampleRate() {
  return sampleRate;
}

uint64_t AsyncDecoder::getPosition() {
  return position;
}

uint64_t AsyncDecoder::getLoopStart() {
  return loopStart;
}

uint64_t AsyncDecoder::getLoopEnd() {
  return loopEnd;
}

bool AsyncDecoder::errorOccured() const {
  return error;
}

void AsyncDecoder::setLooped(bool looped_p) {
  looped = looped_p;
  askSetLooped = true;
}

FrameFormat AsyncDecoder::getFormat() const {
  return format;
}

void AsyncDecoder::asyncThread() {
  while(!ready) {
    std::this_thread::yield() ;
  }
  std::vector<sample_t> vect(Tools::numberOfChannels(format));
  size_t last = vect.size();
  decoder->setLooped(looped);
  while(run) {
    while(true && !ended) {
      if(last == vect.size()) {
        if(!decoder->get(vect)) {
          ended = true;
          break;
        }
        last = 0;
      }

      bool full = false;
      while(last < vect.size()) {
        if(!buffer.tryPush(vect[last])) {
          full = true;
          break;
        }
        last++;
        if(!run) {
          return;
        }
      }

      if(!run) {
        return;
      }
      if(full) {
        break;
      }

    }

    if(decoder->errorOccured()) {
      error = true;
    }

    position = decoder->getPosition();

    if(askSeek) {
      decoder->seek(seekPosition);
      askSeek = false;
      ended = false;
      error = false;
    }
    if(askSetLooped) {
      decoder->setLooped(looped);
      askSetLooped = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}


ResultValue<SoundBuffer> decodeSound(AudioDecoder& decoder) {
  SoundBuffer sound(decoder.getSampleRate(), decoder.getFormat(), decoder.getLength(), decoder.getLoopStart(), decoder.getLoopEnd());
  std::array<sample_t, Tools::MaxNumberOfChannels> frame;
  size_t i = 0;
  while(decoder.get(frame)) {
    sound.setFrame(i, frame.begin());
    i++;
  }
  if(decoder.errorOccured()) {
    return Result::error("error during decoding sound");
  }
  return sound;
}


FileInput::Parameters::Parameters(bool looped_p, Time position_p, double tempo_p, bool async_p) :
  looped(looped_p),
  position(position_p),
  tempo(tempo_p),
  async(async_p) {}

FileInput::FileInput(std::unique_ptr<AudioDecoder> decoder_p, const Parameters& parameters) :
  decoder( ( parameters.async ? std::make_unique<AsyncDecoder>(std::move(decoder_p), Time::seconds(1), parameters.looped) : std::move(decoder_p) ) ), looped(parameters.looped), tempo(parameters.tempo)
{
  if(parameters.position.miliseconds() > 1) {
    decoder->seek(parameters.position.seconds() * decoder->getSampleRate().Hz());
  }
  decoder->setLooped(looped);
}

void FileInput::get(std::span<sample_t> out) {
  std::array<sample_t, Tools::MaxNumberOfChannels> frame;

  while(playing) {
    if(sampleRateConverters.front().outReady()) {
      break;
    }
    if(!decoder->get(frame)) {
      ended = true;
      return;
    }
    for(size_t i = 0; i < sampleRateConverters.size(); i++) {
      sampleRateConverters[i].push(frame[i]);
    }
  }
  for(size_t i = 0; i < sampleRateConverters.size(); i++) {
    out[i] = sampleRateConverters[i].get();
  }

  if(fadingOut) {
    auto v = volume.update();
    for(size_t i = 0; i < sampleRateConverters.size(); i++) {
      out[i] *= v.linear();
    }
    if(!volume.hasChanged()) {
      ended = true;
    }
  }
}

void FileInput::setSampleRate(Frequency sampleRate_p) {
  sampleRate = sampleRate_p;
  sampleRateConverters.resize(Tools::numberOfChannels(decoder->getFormat()), Tools::SampleRateConverter(decoder->getSampleRate(), sampleRate / tempo));  
}

static Time sampleToTime(uint32_t position, Frequency sampleRate) {
  return Time::seconds(position / sampleRate.Hz());
}

static uint32_t timeToSample(Time position, Frequency sampleRate) {
  return position.seconds() * sampleRate.Hz();
}

void FileInput::setParameter(size_t id, ParameterValue value) {
  switch(id) {
    case PlayingID:
      playing = value.getBoolean();
      break;

    case LoopedID:
      looped = value.getBoolean();
      decoder->setLooped(looped);
      ended = false;
      break;

    case PositionID:
      decoder->seek(timeToSample(value.getTime(), decoder->getSampleRate()));
      break;

    case TempoID:
      for(auto& sampleRateConverter : sampleRateConverters) {
        tempo = value.getNonInteger();
        sampleRateConverter.setOutSampleRateNoFilterUpdate(sampleRate / tempo);
      }
      break;

    case FadeOutID:
      fadingOut = true;
      volume = Tools::Smoother<Volume>(sampleRate, volume.getCurrentValue(), Volume::dB(40 / value.getTime().seconds()));
      volume.setDestination(Volume::dB(-40));
      break;

    default:
      assert(false);
  }
}

ParameterValue FileInput::getOutputValue(size_t id) const {
  switch(id) {
    case GetPositionID:
      return ParameterValue::time(sampleToTime(decoder->getPosition(), decoder->getSampleRate()));
      break;

    case GetLengthID:
      return ParameterValue::time(sampleToTime(decoder->getLength(), decoder->getSampleRate()));
      break;

    default:
      assert(false);
      return ParameterValue();
  }
}

bool FileInput::errorOccured() const {
  return decoder->errorOccured();
}

bool FileInput::isPlaying() const {
  return playing && !ended;
}

FrameFormat FileInput::getFormat() const {
  return decoder->getFormat();
}

} // namespace ZAudio