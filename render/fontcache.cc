#include "render/fontcache.h"

#include "core/assert.h"
#include "core/cache.h"

#include "render/fonts.h"

struct FontCache::FontCachePimpl
    : Cache<std::string, Font, FontCache::FontCachePimpl>
{
 public:
  explicit FontCachePimpl(vfs::FileSystem* fs, TextureCache* cache)
      : fs_(fs)
      , cache_(cache)
  {
    ASSERT(fs);
  }

  std::shared_ptr<Font>
  Create(const std::string& file)
  {
    auto ret = std::make_shared<Font>(fs_, cache_, file);
    return ret;
  }

 private:
  vfs::FileSystem*   fs_;
  TextureCache* cache_;
};

FontCache::FontCache(vfs::FileSystem* fs, TextureCache* cache)
{
  pimp = std::make_unique<FontCache::FontCachePimpl>(fs, cache);
}

FontCache::~FontCache() = default;

std::shared_ptr<Font>
FontCache::GetFont(const std::string& path)
{
  return pimp->Get(path);
}
