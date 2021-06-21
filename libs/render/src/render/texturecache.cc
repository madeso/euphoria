#include "render/texturecache.h"

#include "core/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/texture.h"

namespace euphoria::render
{
    struct texture_cache::texture_cache_pimpl
        : core::cache<core::vfs::file_path, texture2d, texture_cache::texture_cache_pimpl>
    {
        explicit texture_cache_pimpl(core::vfs::file_system* fs) : fs_(fs)
        {
            ASSERT(fs);
        }

        [[nodiscard]] std::shared_ptr<texture2d>
        create(const core::vfs::file_path& file) const
        {
            auto ret = std::make_shared<texture2d>();
            ret->load_from_file
            (
                fs_,
                file,
                core::alpha_load::keep,
                texture2d_load_data{}
            );
            return ret;
        }

        core::vfs::file_system* fs_;
    };


    texture_cache::texture_cache(core::vfs::file_system* fs)
    {
        pimpl = std::make_unique<texture_cache::texture_cache_pimpl>(fs);
    }


    texture_cache::~texture_cache() = default;


    std::shared_ptr<texture2d>
    texture_cache::get_texture(const core::vfs::file_path& path) const
    {
        return pimpl->get(path);
    }


    std::shared_ptr<texture2d>
    texture_cache::get_texture(const std::optional<core::vfs::file_path>& path) const
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
