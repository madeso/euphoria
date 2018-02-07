#ifndef EUPHORIA_COMPONENT_H
#define EUPHORIA_COMPONENT_H

#include "core/id.h"

#include <string>
#include <vector>
#include <memory>
#include <map>

/// Contains the data
class Components
{
 public:
  Components()          = default;
  virtual ~Components() = default;

  // todo: add creation data
  virtual void
  Create(IdGenerator::ID object) = 0;

  virtual bool
  Has(IdGenerator::ID object) const = 0;

  virtual void
  Destroy(IdGenerator::ID object) = 0;

  virtual std::vector<IdGenerator::ID>
  List() const = 0;
};

class ComponentStore;

struct ComponentSystemType
{
  const ComponentStore* owner;
  unsigned long         index;
};

class ComponentStore
{
  void
  Add(const std::string& name, std::shared_ptr<Components> system);

  ComponentSystemType
  Lookup(const std::string& name) const;

  void
  Create(ComponentSystemType type, IdGenerator::ID object);

  std::vector<IdGenerator::ID>
  ListObjectsWithComponents(const std::vector<ComponentSystemType>& types);

 private:
  Components&
  Get(ComponentSystemType type);

  std::shared_ptr<Components>
  GetPtr(ComponentSystemType type);

  std::map<std::string, unsigned long> names;
  std::vector<std::shared_ptr<Components>> components;
};


#endif  // EUPHORIA_COMPONENT_H
