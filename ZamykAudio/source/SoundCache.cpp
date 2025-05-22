#include <ZAudio/SoundCache.h>
#include <ZAudio/BufferDecoder.h>

namespace ZAudio {


// CacheSoundID---------------------------------------------------------------------------------------------

CacheSoundID::CacheSoundID(uint32_t id_p) : id(id_p) {}

uint32_t CacheSoundID::get() const {
  return id;
}

uint32_t& CacheSoundID::get() {
  return id;
}

bool CacheSoundID::operator == (const CacheSoundID& oth) const {
  return id == oth.id;
}

// SoundCache---------------------------------------------------------------------------------------------

void SoundCache::addLoadingFunction(const std::string& extension, LoadingFunction loadingFunction) {
  loadingFunctions.insert({extension, loadingFunction});  
}

std::optional<CacheSoundID> SoundCache::add(const std::filesystem::path& path, OpenMode openMode) {
  auto it = loadingFunctions.find(path.extension().string());
  std::string extension = path.extension().string();
  if(it == loadingFunctions.end()) {
    error = "No loader function for extension " + path.extension().string();
    return std::nullopt;
  }

  auto id = CacheSoundID(lastID);
  lastID++;

  if(openMode == OpenMode::PreBuffer) {
    auto res = it->second(path);
    if(!res) {
      error = res.getDescription();
      return std::nullopt;
    }    
    auto sound = decodeSound(*res.get());
    if(!sound) {
      error = sound.getDescription();
      return std::nullopt;
    }    
    preBufferedSounds.insert({id, std::make_shared<SoundBuffer>(std::move(sound.get()))});
  }
  else {
    streamSounds.insert({id, path});    
  }  

  return id;
}

std::unique_ptr<FileInput> SoundCache::getSound(CacheSoundID id, bool playing, bool looped, Time position) {
  auto it1 = preBufferedSounds.find(id);
  auto it2 = streamSounds.find(id);

  if(it1 != preBufferedSounds.end()) {    
    return std::make_unique<FileInput>(std::make_unique<BufferDecoder>(it1->second), FileInput::Parameters(looped, position, 1., false));
  }
  else if(it2 != streamSounds.end()) {
    auto dec = loadingFunctions[it2->second.extension().string()](it2->second);
    if(!dec) {
      error = dec.getDescription();
      return nullptr;
    }    
    return std::make_unique<FileInput>(std::move(dec.get()), FileInput::Parameters(looped, position, 1., true));
  }
  else {
    return nullptr;
  }
}

std::string SoundCache::getError() const {
  return error;
}


} // namespace ZAudio