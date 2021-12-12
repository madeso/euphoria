#include "render/texturecache.h"

#include "core/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/texture.h"

namespace euphoria::render
{
    struct texture_cache::texture_cache_pimpl
        : core::Cache<core::vfs::FilePath, texture2d, texture_cache::texture_cache_pimpl>
    {
        explicit texture_cache_pimpl(core::vfs::FileSystem* fs) : fs_(fs)
        {
            ASSERT(fs);
        }

        [[nodiscard]] std::shared_ptr<texture2d>
        create(const core::vfs::FilePath& file) const
        {
            auto ret = std::make_shared<texture2d>();
            ret->load_from_file
            (
                fs_,
                file,
                core::AlphaLoad::keep,
                texture2d_load_data{}
            );
            return ret;
        }

        core::vfs::FileSystem* fs_;
    };


    texture_cache::texture_cache(core::vfs::FileSystem* fs)
    {
        pimpl = std::make_unique<texture_cache::texture_cache_pimpl>(fs);
    }


    texture_cache::~texture_cache() = default;


    std::shared_ptr<texture2d>
    texture_cache::get_texture(const core::vfs::FilePath& path) const
    {
        return pimpl->get(path);
    }


    std::shared_ptr<texture2d>
    texture_cache::get_texture(const std::optional<core::vfs::FilePath>& path) const
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
