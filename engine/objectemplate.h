#ifndef EUPHORIA_OBJECTTEMPLATE_H
#define EUPHORIA_OBJECTTEMPLATE_H

#include <string>
#include <vector>
#include <memory>
#include <map>

class World;
class TextureCache;
class DukRegistry;
class FileSystem;

struct ObjectCreationArgs
{
  World*        world;
  unsigned int  ent;
  TextureCache* cache;
  DukRegistry*  reg;

  ObjectCreationArgs(
      World*        aworld,
      unsigned int  aent,
      TextureCache* acache,
      DukRegistry*  areg);
};

class ComponentCreator
{
 public:
  virtual ~ComponentCreator() = default;
  virtual void
  CreateComponent(const ObjectCreationArgs& args) = 0;
};

class ObjectTemplate
{
 public:
  void
  SetupObject(const ObjectCreationArgs& args);

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
LoadTemplates(
    ObjectCreator*     temp,
    FileSystem*        fs,
    const std::string& path,
    DukRegistry*       reg);

#endif  // EUPHORIA_OBJECTTEMPLATE_H
