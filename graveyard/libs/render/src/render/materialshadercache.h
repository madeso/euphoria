#pragma once

#include "core/cache.h"
#include "io/vfs_path.h"
#include "assert/assert.h"
#include "render/materialshader.h"

namespace eu::render
{
    struct MaterialShaderCache
        : core::Cache<io::FilePath, MaterialShader, MaterialShaderCache>
    {
        io::FileSystem* file_system;

        MaterialShaderCache(io::FileSystem* fs) : file_system(fs)
        {
            ASSERT(fs);
        }

        [[nodiscard]] std::shared_ptr<MaterialShader>
        create(const io::FilePath& path) const
        {
            auto ret = std::make_shared<MaterialShader>();
            ret->load(file_system, path);
            return ret;
        }
    };

}
