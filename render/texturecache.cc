#include "render/texturecache.h"

#include "core/assert.h"
#include "render/texture.h"
#include "core/cache.h"

namespace euphoria::render
{

struct TextureCache::TextureCachePimpl
    : core::Cache<std::string, Texture2d, TextureCache::TextureCachePimpl>
{
 public:
  explicit TextureCachePimpl(core::vfs::FileSystem* fs)
      : fs_(fs)
  {
    ASSERT(fs);
  }

  std::shared_ptr<Texture2d>
  Create(const std::string& file)
  {
    auto ret = std::make_shared<Texture2d>();
    ret->LoadFromFile(fs_, file, core::AlphaLoad::Keep, Texture2dLoadData{});
    return ret;
  }

 private:
  core::vfs::FileSystem* fs_;
};

TextureCache::TextureCache(core::vfs::FileSystem* fs)
{
  pimp_ = std::make_unique<TextureCache::TextureCachePimpl>(fs);
}

TextureCache::~TextureCache() = default;

std::shared_ptr<Texture2d>
TextureCache::GetTexture(const std::string& path)
{
  return pimp_->Get(path);
}

std::shared_ptr<Texture2d>
TextureCache::GetTextureIfNotEmpty(const std::string& path)
{
  if(!path.empty())
  {
    return GetTexture(path);
  }
  else
  {
    return nullptr;
  }
}

}
