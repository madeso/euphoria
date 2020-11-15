#include "engine/loadworld.h"

#include "core/proto.h"
#include "core/log.h"
#include "core/ecs-systems.h"
#include "core/vfs_path.h"

#include "engine/components.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"

#include "gaf_world.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.loadworld")

namespace euphoria::engine
{
    void
    LoadWorld
    (
        core::vfs::FileSystem* fs,
        core::ecs::World* world,
        DukRegistry* reg,
        const core::vfs::FilePath& path,
        ObjectCreator* creator,
        duk::Context* ctx,
        duk::Duk* duk
    )
    {
        world::World json;
        const auto   err = core::LoadProtoJson(fs, &json, path);
        if(!err.empty())
        {
            LOG_ERROR("Failed to load world components from {0}: {1}", path, err);
        }

        for(const auto& obj: json.objects)
        {
            const auto& name = obj.template_name;
            auto*       t    = creator->FindTemplate(name);
            if(t == nullptr)
            {
                LOG_ERROR("Failed to find template named {0}", name);
                continue;
            }
            t->CreateObject(ObjectCreationArgs {world, reg, ctx, duk});
        }
    }
}  // namespace euphoria::engine
