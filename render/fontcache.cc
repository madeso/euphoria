#include "render/fontcache.h"

#include "core/assert.h"
#include "core/cache.h"

#include "render/fonts.h"

struct FontCache::FontCachePimpl
    : Cache<std::string, Font, FontCache::FontCachePimpl>
{
 public:
  explicit FontCachePimpl(FileSystem* fs, Shader* sh)
      : fs_(fs), shader(sh)
  {
    ASSERT(fs);
  }

  std::shared_ptr<Font>
  Create(const std::string& file)
  {
    auto ret = std::make_shared<Font>(fs_, shader, file);
    return ret;
  }

 private:
  FileSystem* fs_;
  Shader* shader;
};

FontCache::FontCache(FileSystem* fs, Shader* shader)
{
  pimp = std::make_unique<FontCache::FontCachePimpl>(fs, shader);
}

FontCache::~FontCache() = default;

std::shared_ptr<Font>
FontCache::GetFont(const std::string& path)
{
  return pimp->Get(path);
}
