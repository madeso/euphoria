#include "render/fontcache.h"

#include "core/assert.h"
#include "core/cache.h"

#include "render/fonts.h"

struct FontCache::FontCachePimpl
    : Cache<std::string, Font, FontCache::FontCachePimpl>
{
 public:
  explicit FontCachePimpl(FileSystem* fs)
      : fs_(fs)
  {
    ASSERT(fs);
  }

  std::shared_ptr<Font>
  Create(const std::string& file)
  {
    auto ret = std::make_shared<Font>(fs_, file);
    return ret;
  }

 private:
  FileSystem* fs_;
};

FontCache::FontCache(FileSystem* fs)
{
  pimp = std::make_unique<FontCache::FontCachePimpl>(fs);
}

FontCache::~FontCache() = default;

std::shared_ptr<Font>
FontCache::GetFont(const std::string& path)
{
  return pimp->Get(path);
}
