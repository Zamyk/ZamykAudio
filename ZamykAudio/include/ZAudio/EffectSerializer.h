#pragma once

#include <memory>
#include <unordered_map>
#include <filesystem>

#include <ZAudio/TreeDatabase.h>





namespace ZAudio {

class Effect; // forward

} // namespace ZAudio


namespace ZAudio::Tools {


class EffectSerializer {
public:
  static EffectSerializer& instance();  
  void addEffectType(const Effect& effectType);
  Result save(Tools::TreeDatabaseWriter writer, const Effect& effect);
  ResultValue<std::unique_ptr<Effect>> load(TreeDatabaseReader reader);

  Result save(Tools::TreeDatabase& database, const Effect& effect);
  ResultValue<std::unique_ptr<Effect>> load(TreeDatabase& database);  

private:      
  EffectSerializer();
  std::unordered_map<std::string, std::unique_ptr<Effect>> types;
  std::unique_ptr<Effect> getType(const std::string& id);    
};


} // namespace ZAudio::Tools