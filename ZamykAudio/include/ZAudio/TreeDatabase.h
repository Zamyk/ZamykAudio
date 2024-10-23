#pragma once

#include <unordered_map>
#include <optional>
#include <vector>
#include <string>
#include <sstream> 
#include <variant> 
#include <iostream>

#include <ZAudio/StringTools.h>
#include <ZAudio/CommonTypes.h>

#include <pugixml.hpp>

namespace ZAudio::Tools {


class DatabaseNodeID {
public:
  DatabaseNodeID() = default;
  explicit DatabaseNodeID(size_t v_p) : v(v_p) {}

  size_t get() const {
    return v;
  }  

private:
  size_t v = 0;
};


class TreeDatabase {
public:  
  TreeDatabase();
  void clear();
  std::optional<DatabaseNodeID> addChild(DatabaseNodeID parent, const std::string& name);
  std::optional<DatabaseNodeID> getChild(DatabaseNodeID parent, const std::string& name) const;

  template<typename T, size_t N>
  Result getEnumValue(DatabaseNodeID node, const std::string& name, T& value, const std::array<std::pair<T, std::string>, N>& dictionary) const {
    std::string str;
    Result result = getValue(node, name, str);
    if(!result) {
      return result;
    }
    auto it = std::find_if(dictionary.begin(), dictionary.end(), [str = str] (const std::pair<T, std::string>& p) {
      return p.second == str;
    });
    if(it == dictionary.end()) {
      return Result::error("No corresponding string in dictionary, field name: " + name);
    }
    value = it->first;
    return Result::success();
  }

  template<typename T>
  Result getValue(DatabaseNodeID node, const std::string& name, T& v) const {
    auto it = fields[node.get()].find(name);
    if(it == fields[node.get()].end()) {
      return Result::error("No field named" + name);
    }    
    if(!getFromString(it->second, v)) {
      return Result::error("Error parsing " + name + "value");
    }
    return Result::success();    
  }  

  template<typename T, size_t N>
  Result addEnumValue(DatabaseNodeID node, const std::string& name, T value, const std::array<std::pair<T, std::string>, N>& dictionary) {
    auto it = std::find_if(dictionary.begin(), dictionary.end(), [value = value] (const std::pair<T, std::string>& p) {
      return p.first == value;
    });
    if(it == dictionary.end()) {
      return Result::error("No corresponding enum value in dictionary, field name: " + name);
    }
    return addString(node, name, it->second);
  }

  template<typename T>
  Result addValue(DatabaseNodeID node, const std::string& name, const T& value) {
    if constexpr (std::is_fundamental<T>::value) {
      return addString(node, name, std::to_string(value));
    }
    else {
      return addString(node, name, value.toString());
    }    
  }
  
  DatabaseNodeID getRoot() const;

  template<typename Out>
  Result toXml(Out& out) {
    pugi::xml_document doc;
    toXml(0, doc.append_child("xmlDoc"));
    doc.save(out);
    return Result::success();
  }

  template<typename In>
  Result fromXML(In& in) {
    pugi::xml_document doc;
    auto result = doc.load(in);
    if(!result) {
      return Result::error(result.description());
    }
    auto xmlNode = doc.child("xmlDoc");
    tree.push_back({});
    fields.push_back({});
    auto succ = fromXml(0, xmlNode);
    if(!succ) {
      return Result::error("Error");
    }
    return Result::success();
  }

private:
  std::vector<std::unordered_map<std::string, DatabaseNodeID>> tree;
  std::vector<std::unordered_map<std::string, std::string>> fields;

  void toXml(size_t node, pugi::xml_node xmlNode);
  bool fromXml(size_t node, pugi::xml_node xmlNode);
  Result addString(DatabaseNodeID node, const std::string& name, const std::string& value);

  template<typename T>
  bool getFromString(const std::string& str, T& v) const {    
    if constexpr(std::is_integral_v<T>) {
      auto tmp = StringTools::stringToInt(str);      
      if(!tmp) {
        return false;      
      }    
      v = *tmp;
    }
    else if constexpr(std::is_floating_point_v<T>) {
      auto tmp = StringTools::stringToDouble(str);      
      if(!tmp) {
        return false;      
      }    
      v = *tmp;
    }
    else {
      auto tmp = T::fromString(str);
      if(!tmp) {
        return false;      
      }    
      v = *tmp;
    }
    return true;
  }       
};

template<>
Result TreeDatabase::addValue(DatabaseNodeID node, const std::string& name, const std::string& value);

template<>
Result TreeDatabase::addValue(DatabaseNodeID node, const std::string& name, const bool& value);


template<>
bool TreeDatabase::getFromString(const std::string& str, bool& v) const;

template<>
bool TreeDatabase::getFromString(const std::string& str, std::string& v) const;


class TreeDatabaseWriter {
public:
  TreeDatabaseWriter(TreeDatabase* database_p, DatabaseNodeID node_p);
  std::optional<TreeDatabaseWriter> addChild(const std::string& name) const;

  template<typename T>  
  Result addValue(const std::string& name, const T& value) {
    return database->addValue(curr, name, value);
  }

  template<typename T, size_t N>  
  Result addEnumValue(const std::string& name, const T& value, const std::array<std::pair<T, std::string>, N>& dictionary) {
    return database->addEnumValue(curr, name, value, dictionary);
  }

private:
  TreeDatabase* database = nullptr;
  DatabaseNodeID curr;    
};


class TreeDatabaseReader {
public:
  TreeDatabaseReader(const TreeDatabase* database_p, DatabaseNodeID node_p);
  std::optional<TreeDatabaseReader> getChild(std::string name) const;

  template<typename T>
  Result getValue(std::string name, T& v) {
    return database->template getValue<T>(curr, name, v);
  }

  template<typename T, size_t N>  
  Result getEnumValue(std::string name, T& value, const std::array<std::pair<T, std::string>, N>& dictionary) {
    return database->getEnumValue(curr, name, value, dictionary);
  }

private:
  const TreeDatabase* database = nullptr;
  DatabaseNodeID curr;  
};

} // namespace ZAudio::Tools