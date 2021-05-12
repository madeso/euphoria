#include "render/texturecache.h"

#include "core/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/texture.h"

namespace euphoria::render
{
    struct TextureCache::TextureCachePimpl
        : core::cache<core::vfs::file_path, texture2d, TextureCache::TextureCachePimpl>
    {
        explicit TextureCachePimpl(core::vfs::file_system* fs) : fs_(fs)
        {
            ASSERT(fs);
        }

        [[nodiscard]] std::shared_ptr<texture2d>
        Create(const core::vfs::file_path& file) const
        {
            auto ret = std::make_shared<texture2d>();
            ret->load_from_file
            (
                fs_,
                file,
                core::alpha_load::Keep,
                texture2d_load_data{}
            );
            return ret;
        }

        core::vfs::file_system* fs_;
    };


    TextureCache::TextureCache(core::vfs::file_system* fs)
    {
        pimp_ = std::make_unique<TextureCache::TextureCachePimpl>(fs);
    }


    TextureCache::~TextureCache() = default;


    std::shared_ptr<texture2d>
    TextureCache::GetTexture(const core::vfs::file_path& path) const
    {
        return pimp_->get(path);
    }


    std::shared_ptr<texture2d>
    TextureCache::GetTexture(const std::optional<core::vfs::file_path>& path) const
    {
        if(path.has_value())
        {
            return GetTexture(path.value());
        }
        else
        {
            return nullptr;
        }
    }
}
