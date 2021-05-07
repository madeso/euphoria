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
        core::ecs::world* world,
        DukRegistry* reg,
        const core::vfs::FilePath& path,
        ObjectCreator* creator,
    Sol*        ctx)
{
        world::World json;
        const auto   err = core::read_json_to_gaf_struct_or_get_error_message(fs, &json, path);
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
    t->CreateObject(ObjectCreationArgs{world, reg, ctx, ctx});
        }
    }
}  // namespace euphoria::engine
