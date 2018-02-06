#ifndef EUPHORIA_COMPONENTSYSTEM_H
#define EUPHORIA_COMPONENTSYSTEM_H

#include "core/id.h"

#include <vector>

class ComponentSystem
{
 public:
  ComponentSystem()          = default;
  virtual ~ComponentSystem() = default;

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


#endif  // EUPHORIA_COMPONENTSYSTEM_H
