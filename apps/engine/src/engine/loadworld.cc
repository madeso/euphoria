#include "engine/loadworld.h"

#include "core/proto.h"
#include "core/log.h"
#include "core/ecs-systems.h"
#include "core/vfs_path.h"

#include "engine/components.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"

#include "gaf_world.h"

namespace euphoria::engine
{
    void
    load_world
    (
            core::vfs::file_system* fs,
            core::ecs::world* world,
            script_registry* reg,
            const core::vfs::file_path& path,
            object_creator* creator,
            lua* ctx
    )
    {
        world::World json;
        const auto err = core::read_json_to_gaf_struct_or_get_error_message(fs, &json, path);
        if(!err.empty())
        {
            LOG_ERROR("Failed to load world components from {0}: {1}", path, err);
        }

        for(const auto& obj: json.objects)
        {
            const auto& name = obj.template_name;
            auto* t = creator->find_template(name);
            if(t == nullptr)
            {
                LOG_ERROR("Failed to find template named {0}", name);
                continue;
            }
            t->create_object(object_creation_arguments{world, reg, ctx, ctx});
        }
    }
}
