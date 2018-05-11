#ifndef EUPHORIA_OBJECTTEMPLATE_H
#define EUPHORIA_OBJECTTEMPLATE_H

#include "core/entityid.h"

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace game
{
  class Game;
}

class World;
class TextureCache;
class DukRegistry;
class FileSystem;
struct Components;
class Context;

struct ObjectCreationArgs
{
  World*       world;
  DukRegistry* reg;
  Context*     ctx;

  ObjectCreationArgs(World* aworld, DukRegistry* areg, Context* actx);
};

class ComponentCreator
{
 public:
  virtual ~ComponentCreator() = default;
  virtual void
  CreateComponent(const ObjectCreationArgs& args, EntityId id) = 0;
};

class ObjectTemplate
{
 public:
  EntityId
  CreateObject(const ObjectCreationArgs& args);

  std::vector<std::shared_ptr<ComponentCreator>> components;
};

class ObjectCreator
{
 public:
  std::map<std::string, std::shared_ptr<ObjectTemplate>> templates;

  ObjectTemplate*
  FindTemplate(const std::string& name);
};

void
LoadTemplatesButOnlyNames(const game::Game& json, ObjectCreator* temp);

void
LoadTemplates(
    const game::Game& json,
    ObjectCreator*    temp,
    DukRegistry*      reg,
    TextureCache*     cache,
    Components*       components);

#endif  // EUPHORIA_OBJECTTEMPLATE_H
