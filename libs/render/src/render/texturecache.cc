#include "render/texturecache.h"

#include "assert/assert.h"
#include "core/cache.h"
#include "io/vfs_path.h"

#include "render/texture.h"

namespace eu::render
{
    struct TextureCache::TextureCachePimpl
        : core::Cache<io::FilePath, Texture2, TextureCache::TextureCachePimpl>
    {
        explicit TextureCachePimpl(io::FileSystem* fs) : vfs(fs)
        {
            ASSERT(fs);
        }

        [[nodiscard]] std::shared_ptr<Texture2>
        create(const io::FilePath& file) const
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

        io::FileSystem* vfs;
    };


    TextureCache::TextureCache(io::FileSystem* fs)
    {
        pimpl = std::make_unique<TextureCache::TextureCachePimpl>(fs);
    }


    TextureCache::~TextureCache() = default;


    std::shared_ptr<Texture2>
    TextureCache::get_texture(const io::FilePath& path) const
    {
        return pimpl->get(path);
    }


    std::shared_ptr<Texture2>
    TextureCache::get_texture(const std::optional<io::FilePath>& path) const
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
