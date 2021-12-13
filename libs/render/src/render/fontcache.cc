#include "render/fontcache.h"

#include "core/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/font.h"


namespace euphoria::render
{
    struct FontCache::font_cache_pimpl
        : core::Cache<core::vfs::FilePath, DrawableFont, FontCache::font_cache_pimpl>
    {
        explicit font_cache_pimpl(core::vfs::FileSystem* fs, TextureCache* cache)
            : fs_(fs)
            , cache_(cache)
        {
            ASSERT(fs);
        }

        std::shared_ptr<DrawableFont>
        create(const core::vfs::FilePath& file)
        {
            auto ret = std::make_shared<DrawableFont>(fs_, cache_, file);
            return ret;
        }

        core::vfs::FileSystem* fs_;
        TextureCache* cache_;
    };

    FontCache::FontCache(core::vfs::FileSystem* fs, TextureCache* cache)
    {
        pimp = std::make_unique<FontCache::font_cache_pimpl>(fs, cache);
    }

    FontCache::~FontCache() = default;

    std::shared_ptr<DrawableFont>
    FontCache::get_font(const core::vfs::FilePath& path) const
    {
        return pimp->get(path);
    }
}
