#ifndef EUPHORIA_DUKREGISTRY_H
#define EUPHORIA_DUKREGISTRY_H

#include "core/componentsystem.h"
#include "core/duk.h"

#include "engine/components.h"

#include <map>
#include <memory>

typedef unsigned long ComponentId;
typedef unsigned long EntityId;

struct Comp
{
  virtual ~Comp() = default;
  virtual bool
  has(EntityId ent, const EntReg& reg) = 0;
};

template <typename T>
struct TComp : public Comp
{
  bool
  has(EntityId ent, const EntReg& reg) override
  {
    return reg.has<T>(ent);
  }
};

class CScript
{
 public:
  std::map<ComponentId, DukValue> properties;
};

class DukRegistry
{
 public:
  explicit DukRegistry(EntReg* r);

  ComponentId
  getPosition2dId();

  ComponentId
  getSpriteId();

  ComponentId
  CreateNewId(const std::string& name);

  bool
  GetCustomComponentByName(const std::string& name, ComponentId* id);

  std::vector<EntityId>
  entities(const std::vector<ComponentId>& types);

  DukValue
  GetProperty(EntityId ent, ComponentId comp);

  void
  SetProperty(EntityId ent, ComponentId comp, DukValue value);

  template <typename T>
  T*
  GetComponentOrNull(EntityId id)
  {
    if(reg->has<T>(id))
    {
      T& t = reg->get<T>(id);
      return &t;
    }
    else
    {
      return nullptr;
    }
  }

 private:
  template <typename T>
  void
  add()
  {
    auto id  = reg->component<T>();
    auto ptr = std::make_shared<TComp<T>>();
    comps.insert(std::make_pair(id, ptr));
  }

  bool
  hasEntityComponent(ComponentId t, EntityId ent);

  bool
  hasEntityComponent(const std::vector<ComponentId>& types, EntityId ent);

  EntReg* reg;
  std::map<ComponentId, std::shared_ptr<Comp>> comps;
  std::map<std::string, ComponentId>           name_to_component;
  ComponentId last_script_id;
};


#endif  // EUPHORIA_DUKREGISTRY_H
