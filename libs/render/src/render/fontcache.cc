#include "render/fontcache.h"

#include "assert/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/font.h"


namespace eu::render
{
    struct FontCache::FontCachePimpl
        : core::Cache<core::vfs::FilePath, DrawableFont, FontCache::FontCachePimpl>
    {
        explicit FontCachePimpl(core::vfs::FileSystem* fs, TextureCache* c)
            : vfs(fs)
            , cache(c)
        {
            ASSERT(fs);
        }

        std::shared_ptr<DrawableFont>
        create(const core::vfs::FilePath& file)
        {
            auto ret = std::make_shared<DrawableFont>(vfs, cache, file);
            return ret;
        }

        core::vfs::FileSystem* vfs;
        TextureCache* cache;
    };

    FontCache::FontCache(core::vfs::FileSystem* fs, TextureCache* cache)
    {
        pimp = std::make_unique<FontCache::FontCachePimpl>(fs, cache);
    }

    FontCache::~FontCache() = default;

    std::shared_ptr<DrawableFont>
    FontCache::get_font(const core::vfs::FilePath& path) const
    {
        return pimp->get(path);
    }
}
