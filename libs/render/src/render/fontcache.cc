#include "render/fontcache.h"

#include "core/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/font.h"


namespace euphoria::render
{
    struct FontCache::FontCachePimpl
        : core::cache<core::vfs::file_path, Font, FontCache::FontCachePimpl>
    {
        explicit FontCachePimpl(core::vfs::file_system* fs, TextureCache* cache)
            : fs_(fs), cache_(cache)
        {
            ASSERT(fs);
        }

        std::shared_ptr<Font>
        Create(const core::vfs::file_path& file)
        {
            auto ret = std::make_shared<Font>(fs_, cache_, file);
            return ret;
        }

        core::vfs::file_system* fs_;
        TextureCache*          cache_;
    };

    FontCache::FontCache(core::vfs::file_system* fs, TextureCache* cache)
    {
        pimp = std::make_unique<FontCache::FontCachePimpl>(fs, cache);
    }

    FontCache::~FontCache() = default;

    std::shared_ptr<Font>
    FontCache::GetFont(const core::vfs::file_path& path) const
    {
        return pimp->get(path);
    }
}
