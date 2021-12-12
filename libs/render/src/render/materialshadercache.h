#pragma once

#include "core/cache.h"
#include "core/vfs_path.h"
#include "core/assert.h"
#include "render/materialshader.h"

namespace euphoria::render
{
    struct material_shader_cache
        : core::Cache<core::vfs::FilePath, material_shader, material_shader_cache>
    {
        material_shader_cache(core::vfs::FileSystem* fs) : file_system(fs)
        {
            ASSERT(fs);
        }

        [[nodiscard]] std::shared_ptr<material_shader>
        create(const core::vfs::FilePath& path) const
        {
            auto ret = std::make_shared<material_shader>();
            ret->load(file_system, path);
            return ret;
        }

        core::vfs::FileSystem* file_system;
    };

}
