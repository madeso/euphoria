#include "engine/loadworld.h"

#include "core/proto.h"
#include "core/log.h"
#include "core/componentsystem.h"

#include "engine/components.h"
#include "engine/dukregistry.h"
#include "engine/componentcreator.h"

#include "world.pb.h"


LOG_SPECIFY_DEFAULT_LOGGER("engine.loadworld")

void
LoadWorld(
    FileSystem*        fs,
    World*             world,
    TextureCache*      cache,
    DukRegistry*       reg,
    const std::string& path,
    ObjectCreator*     creator)
{
  world::World json;
  const auto   err = LoadProtoJson(fs, &json, path);
  if(!err.empty())
  {
    LOG_ERROR("Failed to load world components from " << path << ": " << err);
  }

  for(const auto& obj : json.objects())
  {
    const auto name = obj.template_();
    auto*      t    = creator->FindTemplate(name);
    if(t == nullptr)
    {
      LOG_ERROR("Failed to find template named " << name);
      continue;
    }
    auto ent = world->reg.create();
    t->SetupObject(ObjectCreationArgs{world, ent, cache, reg});
  }
}
