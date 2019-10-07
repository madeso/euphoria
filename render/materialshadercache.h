
#ifndef EUPHORIA_MATERIALSHADERCACHE_H
#define EUPHORIA_MATERIALSHADERCACHE_H

#include "core/cache.h"
#include "core/assert.h"
#include "render/materialshader.h"

namespace euphoria::render
{
    class MaterialShaderCache
        : public core::Cache<std::string, MaterialShader, MaterialShaderCache>
    {
    public:
        MaterialShaderCache(core::vfs::FileSystem* fs) : fs_(fs)
        {
            ASSERT(fs);
        }

        std::shared_ptr<MaterialShader>
        Create(const std::string& path)
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
