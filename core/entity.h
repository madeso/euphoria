#ifndef EUPHORIA_ENTITY_H
#define EUPHORIA_ENTITY_H

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include "core/entityid.h"
#include "core/assert.h"

#define DEBUG_COMPONENT 1

#if DEBUG_COMPONENT == 1
#define COMPONENT_CONSTRUCTOR_IMPLEMENTATION(X) \
  X::X()                                        \
      : Component(typeid(X))                    \
  {                                             \
  }
#define COMPONENT_CONSTRUCTOR_ARG(X) Component(typeid(X)),
#define COMPONENT_CONSTRUCTOR_DEFINITION(X) X();
#else
#define COMPONENT_CONSTRUCTOR_IMPLEMENTATION(X)
#define COMPONENT_CONSTRUCTOR_DEFINITION(X)
#define COMPONENT_CONSTRUCTOR_ARG(X)
#endif

////////////////////////////////////////////////////////////////////////////////

EntityId
GetValue(EntityId id);

EntityVersion
GetVersion(EntityVersion id);

EntityId
GetId(EntityId id, EntityVersion version);

////////////////////////////////////////////////////////////////////////////////

class Component
{
 public:
#if DEBUG_COMPONENT == 1
  explicit Component(const std::type_info& t);
#endif
  virtual ~Component() = default;

#if DEBUG_COMPONENT == 1
  const std::type_info& type;
#endif
};

////////////////////////////////////////////////////////////////////////////////

struct RegistryEntityCallback
{
 public:
  virtual ~RegistryEntityCallback() = default;

  virtual void
  OnCreated(EntityId ent) = 0;
};

template <typename Func>
struct RegistryEntityCallbackFunction : public RegistryEntityCallback
{
  Func func;

  explicit RegistryEntityCallbackFunction(Func f)
      : func(f)
  {
  }

  void
  OnCreated(EntityId ent) override
  {
    func(ent);
  }
};

template <typename Func>
std::shared_ptr<RegistryEntityCallback>
MakeRegistryEntityCallback(Func f)
{
  return std::make_shared<RegistryEntityCallbackFunction<Func>>(f);
}

////////////////////////////////////////////////////////////////////////////////

struct RegistryImpl;
class Registry
{
 public:
  Registry();
  ~Registry();

  EntityId
  Create();

  void
  PostCreate(EntityId id);

  void
  AddCallback(std::shared_ptr<RegistryEntityCallback> callback);

  bool
  IsAlive(EntityId id) const;

  void
  DestroyEntity(EntityId id);


  ComponentId
  NewComponentType(const std::string& name);

  bool
  GetCustomComponentByName(const std::string& name, ComponentId* id);

  std::shared_ptr<Component>
  GetComponent(EntityId entity, ComponentId component);

  void
  AddComponent(
      EntityId entity, ComponentId component, std::shared_ptr<Component> data);


  std::vector<EntityId>
  View(const std::vector<ComponentId>& components);

  template <typename T>
  T*
  GetComponentOrNull(EntityId entity, ComponentId component)
  {
    auto c = GetComponent(entity, component);
    if(c.get() == nullptr)
    {
      return nullptr;
    }
    else
    {
      ASSERTX(c->type == typeid(T), c->type.name(), typeid(T).name());
      return static_cast<T*>(c.get());
    }
  }

  void
  AddAdded();

  void
  RemoveRemoved();

 private:
  std::unique_ptr<RegistryImpl> impl;
};

////////////////////////////////////////////////////////////////////////////////


#endif  // EUPHORIA_ENTITY_H
