#define DR_FLAC_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION

#include <iostream>
#include <dr_flac.h>
#include <dr_mp3.h>
#include <dr_wav.h>
#include <ZAudio/FileIO.h>

#include "stb_vorbis.c"

namespace ZAudio {

static size_t drOnRead(void* pUserData, void* pBufferOut, size_t bytesToRead) {
  FileInputStream* input = static_cast<FileInputStream*>(pUserData);
  return input->read(static_cast<uint8_t*>(pBufferOut), bytesToRead);
}

static drwav_bool32 drOnSeek(void* pUserData, int offset, drwav_seek_origin origin) {
  FileInputStream* input = static_cast<FileInputStream*>(pUserData);
  FileInputStream::SeekOrigin eorigin = (origin == drwav_seek_origin_start ? FileInputStream::SeekOrigin::Start : FileInputStream::SeekOrigin::Current);
  return input->seek(offset, eorigin);
}

static drwav_bool32 drMp3Onseek(void* pUserData, int offset, drmp3_seek_origin origin) {
  FileInputStream* input = static_cast<FileInputStream*>(pUserData);
  FileInputStream::SeekOrigin eorigin = (origin == drmp3_seek_origin_start ? FileInputStream::SeekOrigin::Start : FileInputStream::SeekOrigin::Current);
  return input->seek(offset, eorigin);
}

static drwav_bool32 drMp3OnTell(void* pUserData, drmp3_int64* pCursor) {
  FileInputStream* input = static_cast<FileInputStream*>(pUserData);
  if(auto ans = input->tell()) {
    *pCursor = *ans;
    return true;
  }
  return false;
}

void drMp3OnMeta(void* pUserData, const drmp3_metadata* pMetadata) {

}


static size_t drFlacOnRead(void* pUserData, void* pBufferOut, size_t bytesToRead) {
  const FlacDecoder::FlacFileInput* flacInput = static_cast<FlacDecoder::FlacFileInput*>(pUserData);
  return flacInput->input->read(static_cast<uint8_t*>(pBufferOut), bytesToRead);
}

static drwav_bool32 drFlacOnSeek(void* pUserData, int offset, drflac_seek_origin origin) {
  const FlacDecoder::FlacFileInput* flacInput = static_cast<FlacDecoder::FlacFileInput*>(pUserData);
  FileInputStream::SeekOrigin eorigin = (origin == drflac_seek_origin_start ? FileInputStream::SeekOrigin::Start : FileInputStream::SeekOrigin::Current);
  return flacInput->input->seek(offset, eorigin);
}

size_t drWriteOnWrite(void* pUserData, const void* pData, size_t bytesToWrite) {
  FileOutputStream* output = static_cast<FileOutputStream*>(pUserData);
  return output->write(static_cast<const uint8_t*>(pData), bytesToWrite);
}

static drwav_bool32 drWriteOnSeek(void* pUserData, int offset, drwav_seek_origin origin) {
  FileOutputStream* output = static_cast<FileOutputStream*>(pUserData);
  FileOutputStream::SeekOrigin eorigin = (origin == drwav_seek_origin_start ? FileOutputStream::SeekOrigin::Start : FileOutputStream::SeekOrigin::Current);
  return output->seek(offset, eorigin);
}

static int64_t parse_time(std::string time) {
  int64_t ans;
  try {
    ans = std::stoll(time);
  }
  catch(...) {
    ans = -1;
  }
  return ans;
}

class FileInputStreamStd : public FileInputStream {
public:
  FileInputStreamStd() = default;

  FileInputStreamStd(const FileInputStreamStd& oth) = delete;
  FileInputStreamStd& operator=(const FileInputStreamStd& oth) = delete;

  FileInputStreamStd(FileInputStreamStd&& oth) {
    file = oth.file;
    oth.file = nullptr;
  }

  FileInputStreamStd& operator=(FileInputStreamStd&& oth) {
    file = oth.file;
    oth.file = nullptr;
    return *this;
  }

  bool open(const std::filesystem::path& path) {
    file = fopen(path.string().c_str(), "rb");
    return file;
  }

  size_t read(uint8_t* data, size_t n) override {
    return fread(data, 1, n, file);
  }

  bool seek(size_t pos, SeekOrigin origin) override {
    return fseek(file, pos, (origin == SeekOrigin::Start ? SEEK_SET : SEEK_CUR)) == 0;
  }

  std::optional<size_t> tell() override {
    auto ans = ftell(file);
    if(ans < 0) {
      return std::nullopt;
    }
    return ans;
  }

  ~FileInputStreamStd() {
    if(file) {
      fclose(file);
    }
  }

private:
  FILE* file = nullptr;
};

class FileOutputStreamStd : public FileOutputStream {
public:
  FileOutputStreamStd() = default;

  FileOutputStreamStd(const FileOutputStreamStd& oth) = delete;
  FileOutputStreamStd& operator=(const FileOutputStreamStd& oth) = delete;

  FileOutputStreamStd(FileOutputStreamStd&& oth) {
    file = oth.file;
    oth.file = nullptr;
  }

  FileOutputStreamStd& operator=(FileOutputStreamStd&& oth) {
    file = oth.file;
    oth.file = nullptr;
    return *this;
  }

  bool open(const std::filesystem::path& path) {
    file = fopen(path.string().c_str(), "wb");
    return file;
  }

  size_t write(const uint8_t* data, size_t n) override {
    return fwrite(data, 1, n, file);
  }

  bool seek(size_t pos, SeekOrigin origin) override {
    return fseek(file, pos, (origin == SeekOrigin::Start ? SEEK_SET : SEEK_CUR)) == 0;
  }

  ~FileOutputStreamStd() {
    if(file) {
      fclose(file);
    }
  }

private:
  FILE* file = nullptr;
};


struct MetadataLoopInfo {
enum struct Type {
  None, LoopStart, LoopEnd
};
  Type type;
  int64_t value = 0;

  static MetadataLoopInfo parse(const std::string& str) {
    auto i = str.find('=');
    if(i != std::string::npos) {
      std::string s1 = str.substr(0, i);
      std::string s2 = str.substr(i + 1);
      if(s1 == "LOOPSTART") {
        auto value = parse_time(s2);
        return MetadataLoopInfo{Type::LoopStart, value};
      }
      else if(s1 == "LOOPEND") {
        auto value = parse_time(s2);
        return MetadataLoopInfo{Type::LoopEnd, value};
      }
    }
    return MetadataLoopInfo{Type::None, 0};
  }
};

static void drFlacOnMeta(void* pUserData, drflac_metadata* pMetadata) {
  FlacDecoder::FlacFileInput* flacInput = static_cast<FlacDecoder::FlacFileInput*>(pUserData);
  if(pMetadata->type == DRFLAC_METADATA_BLOCK_TYPE_VORBIS_COMMENT) {
    std::string vendor = pMetadata->data.vorbis_comment.vendor;
    drflac_vorbis_comment_iterator it;
    drflac_init_vorbis_comment_iterator(&it, pMetadata->data.vorbis_comment.commentCount, pMetadata->data.vorbis_comment.pComments);
    drwav_uint32 length = 0;
    const char* cstr = drflac_next_vorbis_comment(&it, &length);
    while(cstr) {
      std::string str(cstr, length);
      auto l = MetadataLoopInfo::parse(str);
      if(l.type == MetadataLoopInfo::Type::LoopStart) {
        flacInput->loopStart = l.value;
      }
      else if(l.type == MetadataLoopInfo::Type::LoopStart) {
        flacInput->loopEnd = l.value;
      }
      cstr = drflac_next_vorbis_comment(&it, &length);
    }
  }
}


//static drwav_bool32 drFlacOnSeek(void* pUserData, int offset, drwav_seek_origin origin) {
//  const FileInputCallbacks* callbacks = static_cast<FileInputCallbacks*>(pUserData);
//  FileInputCallbacks::SeekOrigin eorigin = (origin == drwav_seek_origin_start ? FileInputCallbacks::SeekOrigin::Start : FileInputCallbacks::SeekOrigin::Current);
//  return callbacks->seek(offset, eorigin);
//}


// WavDecoder---------------------------------------------------------------------------------------------------

ResultValue<std::unique_ptr<AudioDecoder>> WavDecoder::load(const std::filesystem::path& path) {
  auto in = std::make_unique<FileInputStreamStd>();
  if(!in->open(path)) {
    return Result::error("Couldn't open file" + path.string());
  }
  return loadFromStream(std::move(in));
}

ResultValue<std::unique_ptr<AudioDecoder>> WavDecoder::loadFromStream(std::unique_ptr<FileInputStream> fileInput) {
  Result res;
  auto ans = std::make_unique<WavDecoder>(std::move(fileInput), res);
  if(!res) {
    return res;
  }
  return static_cast<std::unique_ptr<AudioDecoder>>(std::move(ans));
}

WavDecoder::WavDecoder(std::unique_ptr<FileInputStream> fileInput_p, Result& result) : fileInput(std::move(fileInput_p)) {
  result = Result::success();
  if (!drwav_init(&wav, drOnRead, drOnSeek, const_cast<void*>(static_cast<const void*>(fileInput.get())), nullptr)) {
    init = false;
    result = Result::error("Error loading sound");
  }
  else {
    init = true;
    if(wav.channels == 1) {
      format = FrameFormat::Mono;
    }
    else if(wav.channels == 2) {
      format = FrameFormat::Stereo;
    }
    else {
      result = Result::error("as for now, only mono and stereo :(, number of channels: " + std::to_string(wav.channels));
    }
  }
}

WavDecoder::~WavDecoder() {
  if(init) {
    drwav_uninit(&wav);
  }
}

bool WavDecoder::get(std::span<sample_t> out) {
  std::array<float, Tools::MaxNumberOfChannels> frame;
  if(drwav_read_pcm_frames_f32(&wav, 1, frame.data()) == 0) {
    if(looped) {
      seek(0);
      if(drwav_read_pcm_frames_f32(&wav, 1, frame.data()) == 0) {
        return false;
      }
    }
    else {
      return false;
    }
  }
  std::copy(frame.cbegin(), frame.cend(), out.begin());
  return true;
}

void WavDecoder::seek(uint64_t position) {
  if(!drwav_seek_to_pcm_frame(&wav, position)) {
    error = true;
  }
}

void WavDecoder::setLooped(bool looped_p) {
  looped = looped_p;
}

uint64_t WavDecoder::getLength() {
  return wav.totalPCMFrameCount;
}

Frequency WavDecoder::getSampleRate() {
  return Frequency::Hz(wav.sampleRate);
}

uint64_t WavDecoder::getPosition() {
  return wav.readCursorInPCMFrames;
}

bool WavDecoder::errorOccured() const {
  return error;
}

FrameFormat WavDecoder::getFormat() const {
  return format;
}


// FlacDecoder---------------------------------------------------------------------------------------------------

FlacDecoder::LoadResult::LoadResult(ResultValue<std::unique_ptr<FlacDecoder>>&& result, bool loopsLoaded_p) :
  ResultValue<std::unique_ptr<FlacDecoder>>(std::move(result)),
  loadedLoopsV(loopsLoaded_p) {}

bool FlacDecoder::LoadResult::loadedLoops() const {
  return loadedLoopsV;
}

FlacDecoder::LoadResult FlacDecoder::load(const std::filesystem::path& path, bool loadLoops) {
  auto in = std::make_unique<FileInputStreamStd>();
  if(!in->open(path)) {
    return LoadResult(Result::error("Couldn't open file" + path.string()), false);
  }
  return loadFromStream(std::move(in), loadLoops);
}

FlacDecoder::LoadResult FlacDecoder::loadFromStream(std::unique_ptr<FileInputStream> fileInput, bool loadLoops) {
  Result res;
  auto ans = std::make_unique<FlacDecoder>(std::move(fileInput), loadLoops, res);
  if(!res) {
    return LoadResult(res, false);
  }
  return LoadResult(std::move(ans), loadLoops);
}

FlacDecoder::FlacDecoder(std::unique_ptr<FileInputStream> fileInput_p, bool& loadLoops, Result& result) : fileInput(std::make_unique<FlacFileInput>(std::move(fileInput_p))) {
  result = Result::success();
  if(loadLoops) {
    flac = drflac_open_with_metadata(drFlacOnRead, drFlacOnSeek, drFlacOnMeta, const_cast<void*>(static_cast<const void*>(fileInput.get())), nullptr);
  }
  else {
    flac = drflac_open(drFlacOnRead, drFlacOnSeek, const_cast<void*>(static_cast<const void*>(fileInput.get())), nullptr);
  }
  if(fileInput->loopStart != -1 && fileInput->loopEnd != -1) {
    loopStart = fileInput->loopStart;
    loopEnd = fileInput->loopEnd;
  }
  else {
    loadLoops = false;
    loopStart = 0;
    loopEnd = getLength();
  }
  if (flac == nullptr) {
    result = Result::error("Error loading flac from callbacks");
  }
  else {
    if(flac->channels == 1) {
      format = FrameFormat::Mono;
    }
    else if(flac->channels == 2) {
      format = FrameFormat::Stereo;
    }
    else {
      result = Result::error("as for now, only mono and stereo :(, number of channels: " + std::to_string(flac->channels));
    }
  }
}

FlacDecoder::~FlacDecoder() {
  drflac_close(flac);
}

bool FlacDecoder::get(std::span<sample_t> out) {
  std::array<float, Tools::MaxNumberOfChannels> frame;
  if(looped && getPosition() >= loopEnd) {
    seek(loopStart);
  }
  if(drflac_read_pcm_frames_f32(flac, 1, frame.data()) == 0) {
    return false;
  }
  std::copy(frame.cbegin(), frame.cend(), out.begin());
  return true;
}

void FlacDecoder::seek(uint64_t position) {
  if(!drflac_seek_to_pcm_frame(flac, position)) {
    error = true;
  }
}

void FlacDecoder::setLooped(bool looped_p) {
  looped = looped_p;
}

uint64_t FlacDecoder::getLength() {
  return flac->totalPCMFrameCount;
}

uint64_t FlacDecoder::getLoopStart() {
  return loopStart;
}
uint64_t FlacDecoder::getLoopEnd() {
  return loopEnd;
}

Frequency FlacDecoder::getSampleRate() {
  return Frequency::Hz(flac->sampleRate);
}

uint64_t FlacDecoder::getPosition() {
  return flac->currentPCMFrame;
}

bool FlacDecoder::errorOccured() const {
  return error;
}

FrameFormat FlacDecoder::getFormat() const {
  return format;
}


// VorbisDecoder ---------------------------------------------------------------------------------------------------


VorbisDecoder::LoadResult::LoadResult(ResultValue<std::unique_ptr<VorbisDecoder>>&& result, bool loopsLoaded_p) :
  ResultValue<std::unique_ptr<VorbisDecoder>>(std::move(result)),
  loadedLoopsV(loopsLoaded_p) {}

bool VorbisDecoder::LoadResult::loadedLoops() const {
  return loadedLoopsV;
}

VorbisDecoder::LoadResult VorbisDecoder::load(const std::filesystem::path& path, bool loadLoops) {
  auto in = std::make_unique<FileInputStreamStd>();
  if(!in->open(path)) {
    return LoadResult(Result::error("Couldn't open file" + path.string()), false);
  }
  return loadFromStream(std::move(in), loadLoops);
}

VorbisDecoder::LoadResult VorbisDecoder::loadFromStream(std::unique_ptr<FileInputStream> fileInput, bool loadLoops) {
  Result res;
  auto ans = std::make_unique<VorbisDecoder>(std::move(fileInput), loadLoops, res);
  if(!res) {
    return LoadResult(res, false);
  }
  return LoadResult(std::move(ans), loadLoops);
}

VorbisDecoder::VorbisDecoder(std::unique_ptr<FileInputStream> fileInput_p, bool& loadLoops, Result& result) : fileInput(std::move(fileInput_p)) {
  result = Result::success();

  data.clear();
  static constexpr size_t BufferSize = 1024;
  std::vector<uint8_t> buffer(BufferSize);
  while(true) {
    size_t got = fileInput->read(buffer.data(), BufferSize);
    data.insert(data.end(), buffer.begin(), buffer.begin() + got);
    if(got < BufferSize) {
      break;
    }
  }

  int error = 0;
  vorbis = stb_vorbis_open_memory(data.data(), data.size(), &error, nullptr);
  if(error != 0) {
    result = Result::error("stb error todo message");
    return;
  }

  // loop info
  {
    auto comments = stb_vorbis_get_comment(vorbis);

    loopStart = loopEnd = -1;

    bool gotStart = false;
    bool gotEnd = false;

    for(int i = 0; i < comments.comment_list_length; i++) {
      std::string str(comments.comment_list[i]);
      auto l = MetadataLoopInfo::parse(str);
      if(l.type == MetadataLoopInfo::Type::LoopStart) {
        loopStart = l.value;
        gotStart = true;
      }
      else if(l.type == MetadataLoopInfo::Type::LoopEnd) {
        loopEnd = l.value;
        gotEnd = true;
      }
    }

    if(gotStart && gotEnd) {
      loadLoops = true;      
    }
    else {
      loadLoops = false;
      loopStart = 0;
      loopEnd = getLength();
    }
  }

  if(vorbis->channels == 1) {
    format = FrameFormat::Mono;
  }
  else if(vorbis->channels == 2) {
    format = FrameFormat::Stereo;
  }
  else {
    result = Result::error("as for now, only mono and stereo :(, number of channels: " + std::to_string(vorbis->channels));
  }
}

VorbisDecoder::~VorbisDecoder() {
  if(vorbis) {
    free(vorbis);
  }
}

bool VorbisDecoder::get(std::span<sample_t> out) {
  std::array<float, Tools::MaxNumberOfChannels> frame;
  if(looped && getPosition() >= loopEnd) {
    seek(loopStart);
  }
  std::array<float*, Tools::MaxNumberOfChannels> buffer;
  for(size_t i = 0; i < Tools::MaxNumberOfChannels; i++) {
    buffer[i] = &frame[i];
  }

  if(stb_vorbis_get_samples_float(vorbis, vorbis->channels, buffer.data(), 1) == 0) {
    return false;
  }
  std::copy(frame.cbegin(), frame.cend(), out.begin());
  return true;
}

void VorbisDecoder::seek(uint64_t position) {
  if(!stb_vorbis_seek(vorbis, position)) {
    error = true;
  }
}

void VorbisDecoder::setLooped(bool looped_p) {
  looped = looped_p;
}

uint64_t VorbisDecoder::getLength() {
  if(vorbis) {
    stb_vorbis_stream_length_in_samples(vorbis);
  }
  else {
    return 0;
  }
}

uint64_t VorbisDecoder::getLoopStart() {
  return loopStart;
}
uint64_t VorbisDecoder::getLoopEnd() {
  return loopEnd;
}

Frequency VorbisDecoder::getSampleRate() {
  return Frequency::Hz(vorbis->sample_rate);
}

uint64_t VorbisDecoder::getPosition() {
  return stb_vorbis_get_sample_offset(vorbis);
}

bool VorbisDecoder::errorOccured() const {
  return error;
}

FrameFormat VorbisDecoder::getFormat() const {
  return format;
}

// Mp3Decoder -----------------------------------------------------------------------------------------------------------------------------------------


ResultValue<std::unique_ptr<AudioDecoder>> Mp3Decoder::load(const std::filesystem::path& path) {
  auto in = std::make_unique<FileInputStreamStd>();
  if(!in->open(path)) {
    return Result::error("Couldn't open file" + path.string());
  }
  return loadFromStream(std::move(in));
}

ResultValue<std::unique_ptr<AudioDecoder>> Mp3Decoder::loadFromStream(std::unique_ptr<FileInputStream> fileInput) {
  Result res;
  auto ans = std::make_unique<Mp3Decoder>(std::move(fileInput), res);
  if(!res) {
    return res;
  }
  return static_cast<std::unique_ptr<AudioDecoder>>(std::move(ans));
}

Mp3Decoder::Mp3Decoder(std::unique_ptr<FileInputStream> fileInput_p, Result& result) : fileInput(std::move(fileInput_p)) {
  result = Result::success();
  if (!drmp3_init(&mp3, drOnRead, drMp3Onseek, drMp3OnTell, drMp3OnMeta, const_cast<void*>(static_cast<const void*>(fileInput.get())), nullptr)) {
    init = false;
    result = Result::error("Error loading mp3 from callbacks");
  }
  else {
    init = true;
    if(mp3.channels == 1) {
      format = FrameFormat::Mono;
    }
    else if(mp3.channels == 2) {
      format = FrameFormat::Stereo;
    }
    else {
      result = Result::error("as for now, only mono and stereo :(, number of channels: " + std::to_string(mp3.channels));
    }
  }
}

Mp3Decoder::~Mp3Decoder() {
  if(init) {
    drmp3_uninit(&mp3);
  }
}

bool Mp3Decoder::get(std::span<sample_t> out) {
  std::array<float, Tools::MaxNumberOfChannels> frame;
  if(drmp3_read_pcm_frames_f32(&mp3, 1, frame.data()) == 0) {
    if(looped) {
      seek(0);
      if(drmp3_read_pcm_frames_f32(&mp3, 1, frame.data()) == 0) {
        return false;
      }
    }
    return false;
  }
  std::copy(frame.cbegin(), frame.cend(), out.begin());
  return true;
}

void Mp3Decoder::seek(uint64_t position) {
  if(!drmp3_seek_to_pcm_frame(&mp3, position)) {
    error = true;
  }
}

void Mp3Decoder::setLooped(bool looped_p) {
  looped = looped_p;
}

uint64_t Mp3Decoder::getLength() {
  return drmp3_get_pcm_frame_count(&mp3);
}

Frequency Mp3Decoder::getSampleRate() {
  return Frequency::Hz(mp3.sampleRate);
}

uint64_t Mp3Decoder::getPosition() {
  return mp3.currentPCMFrame;
}

bool Mp3Decoder::errorOccured() const {
  return error;
}

FrameFormat Mp3Decoder::getFormat() const {
  return format;
}


// WavEncoder---------------------------------------------------------------------------------------------------------------------------------------------------------------------------


ResultValue<std::unique_ptr<AudioEncoder>> WavEncoder::create(Frequency sampleRate_p, FrameFormat format_p, const std::filesystem::path& path) {
  auto out = std::make_unique<FileOutputStreamStd>();
  if(!out->open(path)) {
    return Result::error("Couldn't open file" + path.string());
  }
  return createWithStream(sampleRate_p, format_p, std::move(out));
}

ResultValue<std::unique_ptr<AudioEncoder>> WavEncoder::createWithStream(Frequency sampleRate_p, FrameFormat format_p, std::unique_ptr<FileOutputStream> fileOutput) {
  Result res;
  auto ans = std::make_unique<WavEncoder>(sampleRate_p, format_p, std::move(fileOutput), res);
  if(!res) {
    return res;
  }
  return static_cast<std::unique_ptr<AudioEncoder>>(std::move(ans));
}

WavEncoder::WavEncoder(Frequency sampleRate_p, FrameFormat format_p, std::unique_ptr<FileOutputStream> fileOutput_p, Result& result) :
  fileOutput(std::move(fileOutput_p)),
  sampleRate(sampleRate_p),
  format(format_p)
{
  result = Result::success();
  drwav_data_format wavFormat;
  wavFormat.container = drwav_container_riff;
  wavFormat.format = DR_WAVE_FORMAT_IEEE_FLOAT;
  wavFormat.channels = Tools::numberOfChannels(format);
  wavFormat.sampleRate = sampleRate.Hz();
  wavFormat.bitsPerSample = 32;

  if (!drwav_init_write(&wav, &wavFormat, drWriteOnWrite, drWriteOnSeek, fileOutput.get(), nullptr)) {
    result = Result::error("Error saving wav");
  }
}

WavEncoder::~WavEncoder() {
  drwav_uninit(&wav);
}

Frequency WavEncoder::getSampleRate() const {
  return sampleRate;
}

FrameFormat WavEncoder::getFormat() const {
  return format;
}

void WavEncoder::send(std::span<const sample_t> out) {
  std::array<float, Tools::MaxNumberOfChannels> frame;
  std::copy(out.begin(), out.end(), frame.begin());
  if(drwav_write_pcm_frames(&wav, 1, frame.data()) == 0) {
    error = true;
  }
}

bool WavEncoder::errorOccured() const {
  return error;
}

bool WavEncoder::ended() const {
  return false;
}


} // namespace ZAudio