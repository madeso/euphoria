#ifndef EUPHORIA_COMPONENTSYSTEMSTORE_H
#define EUPHORIA_COMPONENTSYSTEMSTORE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "core/id.h"

class ComponentSystem;

class ComponentSystemStore;

struct ComponentSystemType
{
  const ComponentSystemStore* owner;
  int                         index;
};

class ComponentSystemStore
{
 public:
  ComponentSystemStore() = default;

  void
  Add(const std::string& name, std::shared_ptr<ComponentSystem> system);

  ComponentSystemType
  Lookup(const std::string& name) const;

  void
  Create(ComponentSystemType type, IdGenerator::ID object);

 private:
  std::map<std::string, int> names;
  std::vector<std::shared_ptr<ComponentSystem>> systems;
};


#endif  // EUPHORIA_COMPONENTSYSTEMSTORE_H
