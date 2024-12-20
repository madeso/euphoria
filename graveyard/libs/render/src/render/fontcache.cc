#include "render/fontcache.h"

#include "assert/assert.h"
#include "core/cache.h"
#include "io/vfs_path.h"

#include "render/font.h"


namespace eu::render
{
    struct FontCache::FontCachePimpl
        : core::Cache<io::FilePath, DrawableFont, FontCache::FontCachePimpl>
    {
        explicit FontCachePimpl(io::FileSystem* fs, TextureCache* c)
            : vfs(fs)
            , cache(c)
        {
            ASSERT(fs);
        }

        std::shared_ptr<DrawableFont>
        create(const io::FilePath& file)
        {
            auto ret = std::make_shared<DrawableFont>(vfs, cache, file);
            return ret;
        }

        io::FileSystem* vfs;
        TextureCache* cache;
    };

    FontCache::FontCache(io::FileSystem* fs, TextureCache* cache)
    {
        pimp = std::make_unique<FontCache::FontCachePimpl>(fs, cache);
    }

    FontCache::~FontCache() = default;

    std::shared_ptr<DrawableFont>
    FontCache::get_font(const io::FilePath& path) const
    {
        return pimp->get(path);
    }
}
