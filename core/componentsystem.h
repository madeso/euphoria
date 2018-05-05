#ifndef EUPHORIA_COMPONENTSYSTEM_H
#define EUPHORIA_COMPONENTSYSTEM_H

#include <memory>

#include <string>
#include <vector>

#include "core/entityid.h"

#include <entt/entity/registry.hpp>

typedef entt::Registry<Entity> EntReg;

class Systems;

// its horrible to reference in another module,
// but for now we only need a pointer to pass around
// todo: fix this by merging in more of the renderer into core?
class SpriteRenderer;

// only a base class container
class ComponentSystem
{
 public:
  explicit ComponentSystem(const std::string& the_name);
  virtual ~ComponentSystem() = default;

  virtual void
  RegisterCallbacks(Systems* systems) = 0;

  // for debug purposes
  const std::string name;
};

class ComponentSystemStore
{
 public:
  ComponentSystemStore() = default;

  void
  Add(std::shared_ptr<ComponentSystem> system);

 private:
  std::vector<std::shared_ptr<ComponentSystem>> systems;
};

class ComponentSystemUpdate
{
 public:
  virtual void
  Update(EntReg* reg, float dt) const = 0;
};

class ComponentSystemSpriteDraw
{
 public:
  virtual void
  Draw(EntReg* reg, SpriteRenderer* renderer) const = 0;
};

template <typename TSystem>
class SystemStore
{
 public:
  void
  Add(TSystem* system)
  {
    systems.emplace_back(system);
  }

 protected:
  std::vector<TSystem*> systems;
};

class ComponentSystemUpdateStore : public SystemStore<ComponentSystemUpdate>
{
 public:
  void
  Update(EntReg* reg, float dt) const;
};

class ComponentSystemSpriteDrawStore
    : public SystemStore<ComponentSystemSpriteDraw>
{
 public:
  void
  Draw(EntReg* reg, SpriteRenderer* renderer) const;
};

class Systems
{
 public:
  void
  AddAndRegister(std::shared_ptr<ComponentSystem> system);

  // stores the system
  ComponentSystemStore store;

  // system references for various global callbacks
  ComponentSystemUpdateStore     update;
  ComponentSystemSpriteDrawStore spriteDraw;
};

class World
{
 public:
  EntReg   reg;
  Systems* systems;

  World(Systems* sys);

  void
  Update(float dt);

  void
  Draw(SpriteRenderer* renderer);
};

#endif  // EUPHORIA_COMPONENTSYSTEM_H
