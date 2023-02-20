#pragma once

#include "core/cache.h"
#include "core/vfs_path.h"
#include "assert/assert.h"
#include "render/materialshader.h"

namespace euphoria::render
{
    struct MaterialShaderCache
        : core::Cache<core::vfs::FilePath, MaterialShader, MaterialShaderCache>
    {
        core::vfs::FileSystem* file_system;

        MaterialShaderCache(core::vfs::FileSystem* fs) : file_system(fs)
        {
            ASSERT(fs);
        }

        [[nodiscard]] std::shared_ptr<MaterialShader>
        create(const core::vfs::FilePath& path) const
        {
            auto ret = std::make_shared<MaterialShader>();
            ret->load(file_system, path);
            return ret;
        }
    };

}
