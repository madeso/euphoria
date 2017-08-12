#include "render/texturecache.h"

#include "core/assert.h"
#include "render/texture.h"
#include "core/cache.h"

struct TextureCache::TextureCachePimpl
    : Cache<std::string, Texture2d, TextureCache::TextureCachePimpl>
{
 public:
  TextureCachePimpl(FileSystem* fs)
      : fs_(fs)
  {
    Assert(fs);
  }

  std::shared_ptr<Texture2d>
  Create(const std::string& file)
  {
    auto ret = std::make_shared<Texture2d>();
    ret->LoadFromFile(fs_, file, AlphaLoad::Keep, Texture2dLoadData{});
    return ret;
  }

 private:
  FileSystem* fs_;
};

TextureCache::TextureCache(FileSystem* fs)
{
  pimp_.reset(new TextureCachePimpl{fs});
}

TextureCache::~TextureCache()
{
}

std::shared_ptr<Texture2d>
TextureCache::GetTexture(const std::string& path)
{
  return pimp_->Get(path);
}
