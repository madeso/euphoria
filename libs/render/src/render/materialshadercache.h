
#ifndef EUPHORIA_MATERIALSHADERCACHE_H
#define EUPHORIA_MATERIALSHADERCACHE_H

#include "core/cache.h"
#include "core/vfs_path.h"
#include "core/assert.h"
#include "render/materialshader.h"

namespace euphoria::render
{
    struct MaterialShaderCache
        : public core::cache<core::vfs::FilePath, MaterialShader, MaterialShaderCache>
    {
    public:
        MaterialShaderCache(core::vfs::FileSystem* fs) : fs_(fs)
        {
            ASSERT(fs);
        }

        std::shared_ptr<MaterialShader>
        Create(const core::vfs::FilePath& path)
        {
            auto ret = std::make_shared<MaterialShader>();
            ret->Load(fs_, path);
            return ret;
        }

    private:
        core::vfs::FileSystem* fs_;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_MATERIALSHADERCACHE_H
