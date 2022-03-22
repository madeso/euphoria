#include "engine/loadworld.h"

#include "log/log.h"

#include "core/proto.h"
#include "core/vfs_path.h"

#include "engine/ecs.systems.h"
#include "engine/components.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"

#include "gaf_world.h"
#include "gaf_rapidjson_world.h"

namespace euphoria::engine
{
    void
    load_world
    (
            core::vfs::FileSystem* fs,
            World* world,
            ScriptRegistry* reg,
            const core::vfs::FilePath& path,
            ObjectCreator* creator,
            LuaState* ctx
    )
    {
        const auto json = core::get_default_but_log_errors
        (
            core::read_json_file_to_gaf_struct<world::World>(fs, path, world::ReadJsonWorld)
        );

        for(const auto& obj: json.objects)
        {
            const auto& name = obj.template_name;
            auto* t = creator->find_template(name);
            if(t == nullptr)
            {
                LOG_ERROR("Failed to find template named {0}", name);
                continue;
            }
            t->create_object(ObjectCreationArguments{world, reg, ctx, ctx});
        }
    }
}
