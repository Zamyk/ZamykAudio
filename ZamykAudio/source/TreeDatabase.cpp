#include <ZAudio/TreeDatabase.h>


namespace ZAudio::Tools {


// TreeDatabase --------------------------------------------------

TreeDatabase::TreeDatabase() : tree(1), fields(1) {}

void TreeDatabase::clear() {
  tree.clear();
  fields.clear();
  tree.resize(1);
  fields.resize(1);
}

std::optional<DatabaseNodeID> TreeDatabase::addChild(DatabaseNodeID parent, const std::string& name) {
  if(tree[parent.get()].count(name) > 0) {
    return std::nullopt;      
  }
  DatabaseNodeID id(tree.size());        
  tree.push_back({});
  fields.push_back({});    
  tree[parent.get()].insert({name, id});    
  return id;
}  

std::optional<DatabaseNodeID> TreeDatabase::getChild(DatabaseNodeID parent, const std::string& name) const {
  auto it = tree[parent.get()].find(name);
  if(it == tree[parent.get()].end()) {
    return std::nullopt;
  }
  return it->second;
}

template<>
Result TreeDatabase::addValue(DatabaseNodeID node, const std::string& name, const std::string& value) {
  return addString(node, name, value);
}    

template<>
Result TreeDatabase::addValue(DatabaseNodeID node, const std::string& name, const bool& value) {
  return addString(node, name, (value ? "true" : "false"));
}

DatabaseNodeID TreeDatabase::getRoot() const {    
  return DatabaseNodeID(0);
}

void TreeDatabase::toXml(size_t node, pugi::xml_node xmlNode) {    
  for(auto& field : fields[node]) {
    xmlNode.append_attribute(field.first.c_str()) = field.second.c_str();
  }
  for(auto& child : tree[node]) {      
    pugi::xml_node xmlChild = xmlNode.append_child(child.first.c_str());
    toXml(child.second.get(), xmlChild);  
  }    
}

bool TreeDatabase::fromXml(size_t node, pugi::xml_node xmlNode) {
  for (pugi::xml_attribute attr : xmlNode.attributes()) {
    std::string name = attr.name();
    std::string value = attr.value();            
    fields[node].insert({name, value});
  }
  for (pugi::xml_node xmlChild : xmlNode.children()) {
    std::string name = xmlChild.name();
    auto child = addChild(DatabaseNodeID(node), name);
    if(!child) {
      return false;
    }
    if(!fromXml(child->get(), xmlChild)) {
      return false;
    }
  }
  return true;          
}
  
Result TreeDatabase::addString(DatabaseNodeID node, const std::string& name, const std::string& value) {
  if(fields[node.get()].count(name) > 0) {
    return Result::error("Field " + name + " already exists");
  }    
  fields[node.get()].insert({name, value});    
  return Result::success();
}

template<>
bool TreeDatabase::getFromString(const std::string& str, bool& v) const {
  if(StringTools::caseInsensitiveEqual(str, "true")) {
    v = true;
    return true;
  }
  if(StringTools::caseInsensitiveEqual(str, "false")) {
    v = false;
    return true;      
  }        
  return false;
}

template<>
bool TreeDatabase::getFromString(const std::string& str, std::string& v) const {
  v = str;
  return true;
}   

// TreeDataBaseWriter ---------------------------------------------------------------------------
TreeDatabaseWriter::TreeDatabaseWriter(TreeDatabase* database_p, DatabaseNodeID node_p) : 
  database(database_p),
  curr(node_p) {}

std::optional<TreeDatabaseWriter> TreeDatabaseWriter::addChild(const std::string& name) const {
  auto child = database->addChild(curr, name);
  if(!child) {
    return std::nullopt;
  }
  return TreeDatabaseWriter(database, *child);
}    

// TreeDataBaseReader ---------------------------------------------------------------------------
TreeDatabaseReader::TreeDatabaseReader(const TreeDatabase* database_p, DatabaseNodeID node_p) :
  database(database_p),
  curr(node_p) {}

std::optional<TreeDatabaseReader> TreeDatabaseReader::getChild(std::string name) const {
  auto node = database->getChild(curr, name);
  if(!node) {
    return std::nullopt;
  }
  return TreeDatabaseReader(database, *node);
}  

} // namespace ZAudio::Tools