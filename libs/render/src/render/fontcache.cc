#include "render/fontcache.h"

#include "core/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/font.h"


namespace euphoria::render
{
    struct font_cache::font_cache_pimpl
        : core::Cache<core::vfs::FilePath, drawable_font, font_cache::font_cache_pimpl>
    {
        explicit font_cache_pimpl(core::vfs::FileSystem* fs, texture_cache* cache)
            : fs_(fs)
            , cache_(cache)
        {
            ASSERT(fs);
        }

        std::shared_ptr<drawable_font>
        create(const core::vfs::FilePath& file)
        {
            auto ret = std::make_shared<drawable_font>(fs_, cache_, file);
            return ret;
        }

        core::vfs::FileSystem* fs_;
        texture_cache* cache_;
    };

    font_cache::font_cache(core::vfs::FileSystem* fs, texture_cache* cache)
    {
        pimp = std::make_unique<font_cache::font_cache_pimpl>(fs, cache);
    }

    font_cache::~font_cache() = default;

    std::shared_ptr<drawable_font>
    font_cache::get_font(const core::vfs::FilePath& path) const
    {
        return pimp->get(path);
    }
}
