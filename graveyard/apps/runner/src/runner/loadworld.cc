#include "runner/loadworld.h"

#include "log/log.h"

#include "io/vfs_path.h"

#include "runner/ecs.systems.h"
#include "runner/components.h"
#include "runner/dukregistry.h"
#include "runner/objectemplate.h"

#include "files/world.h"

namespace eu::runner
{
    void
    load_world
    (
        io::FileSystem* fs,
        World* world,
        ScriptRegistry* reg,
        const io::FilePath& path,
        ObjectCreator* creator,
        LuaState* ctx
    )
    {
        files::world::World json;
        
        if (const auto loaded = io::read_json_file(fs, path); loaded == false)
        {
            LOG_ERROR("Failed to load {}: {}", path, loaded.get_error().display);
            return;
        }
        else
        {
            const auto parsed = files::world::parse(log::get_global_logger(), &json, loaded.get_value().root, &loaded.get_value().doc);
            if (!parsed)
            {
                return;
            }
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
            t->create_object(ObjectCreationArguments{world, reg, ctx, ctx});
        }
    }
}
