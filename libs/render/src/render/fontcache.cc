#include "render/fontcache.h"

#include "core/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/font.h"

namespace euphoria::render
{
    struct FontCache::FontCachePimpl
        : core::Cache<core::vfs::FilePath, Font, FontCache::FontCachePimpl>
    {
    public:
        explicit FontCachePimpl(core::vfs::FileSystem* fs, TextureCache* cache)
            : fs_(fs), cache_(cache)
        {
            ASSERT(fs);
        }

        std::shared_ptr<Font>
        Create(const core::vfs::FilePath& file)
        {
            auto ret = std::make_shared<Font>(fs_, cache_, file);
            return ret;
        }

    private:
        core::vfs::FileSystem* fs_;
        TextureCache*          cache_;
    };

    FontCache::FontCache(core::vfs::FileSystem* fs, TextureCache* cache)
    {
        pimp = std::make_unique<FontCache::FontCachePimpl>(fs, cache);
    }

    FontCache::~FontCache() = default;

    std::shared_ptr<Font>
    FontCache::GetFont(const core::vfs::FilePath& path)
    {
        return pimp->Get(path);
    }

}  // namespace euphoria::render
