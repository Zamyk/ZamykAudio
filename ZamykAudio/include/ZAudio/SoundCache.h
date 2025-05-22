#pragma once

#include <filesystem>
#include <functional>
#include <ZAudio/CommonTypes.h>
#include <ZAudio/SoundBuffer.h>
#include <ZAudio/AudioInput.h>
#include <ZAudio/AudioDecoder.h>

namespace ZAudio {

class CacheSoundID {
public:
  CacheSoundID() = default;
  explicit CacheSoundID(uint32_t id_p);
  uint32_t get() const;
  uint32_t& get();
  bool operator == (const CacheSoundID& oth) const;
private:
  uint32_t id = 0;
};


} // namespace ZAudio


template <>
struct std::hash<ZAudio::CacheSoundID> {
  std::size_t operator()(const ZAudio::CacheSoundID& k) const {
    return std::hash<uint32_t>()(k.get());
  }
};


namespace ZAudio {


class SoundCache {
public:
using LoadingFunction = std::function<ResultValue<std::unique_ptr<AudioDecoder>>(const std::filesystem::path&)>;
enum struct OpenMode {
  Stream, PreBuffer
};
  void addLoadingFunction(const std::string& extension, LoadingFunction loadingFunction);
  std::optional<CacheSoundID> add(const std::filesystem::path& path, OpenMode openMode = SoundCache::OpenMode::PreBuffer);
  std::unique_ptr<FileInput> getSound(CacheSoundID id, bool playing = true, bool looped = false, Time position = Time::seconds(0));
  std::string getError() const;
private:
  std::unordered_map<std::string, LoadingFunction> loadingFunctions;
  std::unordered_map<CacheSoundID, std::shared_ptr<SoundBuffer>> preBufferedSounds;
  std::unordered_map<CacheSoundID, std::filesystem::path> streamSounds;
  uint32_t lastID = 0;
  std::string error;
};


} // namespace ZAudio