#include "runner/loadworld.h"

#include "log/log.h"

#include "core/proto.h"
#include "core/vfs_path.h"

#include "runner/ecs.systems.h"
#include "runner/components.h"
#include "runner/dukregistry.h"
#include "runner/objectemplate.h"

#include "gaf_world.h"
#include "gaf_rapidjson_world.h"

namespace euphoria::runner
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
