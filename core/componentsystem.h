#ifndef EUPHORIA_COMPONENTSYSTEM_H
#define EUPHORIA_COMPONENTSYSTEM_H

#include <memory>
#include <string>
#include <vector>

// only a base class container
class ComponentSystem
{
 public:
  explicit ComponentSystem(const std::string& the_name);
  virtual ~ComponentSystem() = default;

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
  Update(float dt) const = 0;
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
  Update(float dt) const;
};

class Systems
{
 public:
  ComponentSystemStore       store;
  ComponentSystemUpdateStore update;
};

#endif  // EUPHORIA_COMPONENTSYSTEM_H
