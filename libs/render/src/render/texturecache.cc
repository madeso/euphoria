#include "render/texturecache.h"

#include "assert/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/texture.h"

namespace euphoria::render
{
    struct TextureCache::texture_cache_pimpl
        : core::Cache<core::vfs::FilePath, Texture2, TextureCache::texture_cache_pimpl>
    {
        explicit texture_cache_pimpl(core::vfs::FileSystem* fs) : vfs(fs)
        {
            ASSERT(fs);
        }

        [[nodiscard]] std::shared_ptr<Texture2>
        create(const core::vfs::FilePath& file) const
        {
            auto ret = std::make_shared<Texture2>();
            ret->load_from_file
            (
                vfs,
                file,
                core::AlphaLoad::keep,
                Texture2dLoadData{}
            );
            return ret;
        }

        core::vfs::FileSystem* vfs;
    };


    TextureCache::TextureCache(core::vfs::FileSystem* fs)
    {
        pimpl = std::make_unique<TextureCache::texture_cache_pimpl>(fs);
    }


    TextureCache::~TextureCache() = default;


    std::shared_ptr<Texture2>
    TextureCache::get_texture(const core::vfs::FilePath& path) const
    {
        return pimpl->get(path);
    }


    std::shared_ptr<Texture2>
    TextureCache::get_texture(const std::optional<core::vfs::FilePath>& path) const
    {
        if(path.has_value())
        {
            return get_texture(path.value());
        }
        else
        {
            return nullptr;
        }
    }
}
