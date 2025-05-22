#include <iostream>
#include <filesystem>
#include <string>
#include <future>
#include <random>

#include <ZAudio/ZAudioInclude.h>
#include <ZAudio/EffectsInclude.h>
#include <ZAudio/SDL_IO.h>
#include <ZAudio/FileIO.h>


using namespace ZAudio;


// this class is made to just play one file easly
class Player {
public:  
  // sample rate is set to 44100
  Player() : 
    engine(Frequency::Hz(44100)),
    mixer(engine.addMixer(FrameFormat::Stereo))
  {
    // now sound cache will be able to load these formats    
    soundCache.addLoadingFunction(".wav", WavDecoder::load);

    soundCache.addLoadingFunction(".flac",
      [](const std::filesystem::path& path) -> ResultValue<std::unique_ptr<AudioDecoder>> {
        auto ans = FlacDecoder::load(path);
        if(ans) {
          return static_cast<std::unique_ptr<AudioDecoder>>(std::move(ans.get()));
        }
        else {
          return Result::error(ans.getDescription());
        }
      }
    );

    soundCache.addLoadingFunction(".mp3", Mp3Decoder::load);    
  }

  Result init() {        
    if(!io.init(Frequency::Hz(44100))) {
      return Result::error(io.getError());
    }

    //static constexpr uint32_t BufferSize = 2048;
    auto sdlOut = io.createDefaultOutput(FrameFormat::Stereo);
    if(!sdlOut) {      
      return Result::error(io.getError());
    }    


    engine.addMixerOutput(mixer, engine.addOutput(std::move(sdlOut)));    
    volume = engine.addEffect<MonoToStereoAdapter>(VolumeControlEffect(VolumeControlEffect::Parameters(Volume::dB(0)))); // VolumeControlEffect is mono so it would mix stereo to mono, so MonoToStereoAdapter is used
    engine.setMixerEffect(mixer, volume);
    return Result::success();
  }

  Result play(const std::filesystem::path& path) {  
    // handle is == InputHandle() if not initalized yet, else stop current music
    if(handle != InputHandle()) {
      engine.stop(mixer, handle);
    }
    

    // normally just use add once and then only getSound, but add doesn't do anything if already sound is in cache so thats not a problem
    auto tmp = soundCache.add(path, SoundCache::OpenMode::Stream);
    if(!tmp) {
      return Result::error("cannot load file: " + path.string());
    }

    auto sound = soundCache.getSound(*tmp);
    if(!sound) {
      return Result::error("cannot load file: " + path.string());
    }
    handle = engine.addInput(std::move(sound));
    engine.play(mixer, handle);    
    return Result::success();
  }

  void stop() {
    engine.stop(mixer, handle);
  }

  void pause() {
    paused_ = true;
    engine.setInputParameter(handle, FileInput::PlayingID, ParameterValue::boolean(false));
  }

  void unpause() {
    paused_ = false;
    engine.setInputParameter(handle, FileInput::PlayingID, ParameterValue::boolean(true));
  }

  bool ended() {
    if(handle == InputHandle()) {
      return true;
    }
    return !paused_ && !engine.isPlaying(handle);
  }

  Time getDuration() {
    return engine.getOutputValue(handle, FileInput::GetLengthID).getTime();
  }

  void seek(Time time) {
    engine.setInputParameter(handle, FileInput::PositionID, ParameterValue::time(time));
  }  

  void setVolume(Volume vol) {
    engine.setEffectParameter(volume, VolumeControlEffect::VolumeChangeID, ParameterValue::volume(vol));
  }
private:
  bool paused_ = false;
  InputHandle handle;
  AudioEngine engine;
  MixerHandle mixer;  
  SoundCache soundCache;
  EffectHandle volume;
  SDL_IO io;
};

std::vector<std::filesystem::path> getFiles() {
  std::array<std::string, 3> extensions = {".wav", ".flac", ".mp3"};
  std::vector<std::filesystem::path> ans;
  for(const auto& f : std::filesystem::directory_iterator(std::filesystem::current_path())) {
    if(std::find(extensions.begin(), extensions.end(), f.path().extension()) != extensions.end()) {
      ans.push_back(f.path());
    }
  }
  return ans;
}

void help() {
  std::cout << 
      "-list => displays music in current folder\n"
      "-play *number* => play song *number*\n"
      "-pause  => pause song\n"
      "-unpause  => unpause song\n"
      "-seek *second*  => seek to second\n"
      "-skip  => skip to next song, works if auto play or shuffle is on\n"
      "-auto  => plays with order of list\n"
      "-shuffle  => plays with random order\n"
      "-quit  => quits player\n"
      "-volume value => set volume reduction in db, range <-96, 0>"
      << std::endl;      
}

void list(const std::vector<std::filesystem::path>& files) {
  for(size_t i = 0; i < files.size(); i++) {
    std::cout << "(" << i << ") " << files[i].filename() << std::endl;
  }
}

int randomInt(int l, int r) {
  static std::random_device device;
  static std::mt19937 gen(device());
  std::uniform_int_distribution<int> dist(l, r);
  return dist(gen);
}

struct FolderPlayer {
  Player player;
  bool shuffle = false;
  bool autoPlay = false;
  size_t songIndex = 0;
  bool paused = true;
  std::vector<std::filesystem::path> files;
  int playing = 0;
  int skip = false;
  bool run = true;
  

  FolderPlayer() {
    if(auto res = player.init(); !res) {
      std::cout << res.getDescription();    
    }
    files = getFiles();
    help();    
  }

  void handleUserCommand(std::string str) {    
    if(str == "-play") {
      shuffle = false;
      std::cin >> str;
      auto tmp = StringTools::stringToInt(str);
      if(!tmp) {
        std::cout << "\nShould be integer!" << std::endl;
        return;
      }        
      playing = *tmp;
      player.play(files[playing]);
      std::cout << "Playing " << files[playing].filename().string() << " " << player.getDuration().minutes() << " minutes" << std::endl;
    }
    if(str == "-pause") {      
      player.pause();
    }
    if(str == "-unpause") {      
      player.unpause();
    }
    if(str == "-seek") {
      std::cin >> str;
      auto tmp = StringTools::stringToDouble(str);
      if(!tmp) {
        std::cout << "\nShould be double!" << std::endl;
        return;
      }
      player.seek(Time::seconds(*tmp));
    }
    if(str == "-skip") {
      skip = true;
    }
    if(str == "-shuffle") {
      autoPlay = false;
      shuffle = true;
    }
    if(str == "-auto") {
      autoPlay = true;
      shuffle = false;
    }
    if(str == "-help") {
      help();
    }
    if(str == "-list") {
      list(files);
    }      
    if(str == "-volume") {
      std::string str;
      std::cin >> str;
      auto tmp = StringTools::stringToDouble(str);
      if(!tmp) {
        std::cout << "\nShould be double!" << std::endl;
        return;
      }
      if(!(*tmp <= 0. || *tmp >= -96.)) {
        std::cout << "\nShould be in range <-96, 0>" << std::endl;
        return;
      }
      Volume vol = Volume::dB(*tmp);
      player.setVolume(vol);
    }
    if(str == "-quit") {
      run = false;
    }    
  }

  void play() {
    auto getFromCmd =
    []() {
      std::string str;
      std::cin >> str;
      return str;
    };

    auto command = std::async(getFromCmd);
    while(run) {
      skip = false;
      auto status = command.wait_for(std::chrono::milliseconds(5));
      if(status == std::future_status::ready) {
        handleUserCommand(command.get());
        if(!run) {
          break;
        }
        command = std::async(getFromCmd);
      }    
      if((player.ended() || skip) && (autoPlay || shuffle)) {
        playing++;
        if(playing == static_cast<int>(files.size())) {
          playing = 0;
        }
        if(shuffle) {
          playing = randomInt(0, files.size() - 1);
        }      
        auto res = player.play(files[playing]);
        if(!res) {
          std::cout << res.getDescription() << "\n";
          return;
        }
        std::cout << "Playing " << files[playing].filename().string() << " " << player.getDuration().minutes() << " minutes" << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }        
  }
};

int main(int argc, char** argv) {
  FolderPlayer p;
  p.play();
}