#ifndef SPACETYPER_TEXTURECACHE_H
#define SPACETYPER_TEXTURECACHE_H

#include <memory>
#include <string>

namespace euphoria::core
{
  namespace vfs
  {
    class FileSystem;
  }
}

namespace euphoria::render
{

  class Texture2d;

  class TextureCache
  {
  public:
    explicit TextureCache(core::vfs::FileSystem* fs);
    ~TextureCache();

    std::shared_ptr<Texture2d>
    GetTexture(const std::string& path);

    std::shared_ptr<Texture2d>
    GetTextureIfNotEmpty(const std::string& path);

  private:
    struct TextureCachePimpl;
    std::unique_ptr<TextureCachePimpl> pimp_;
  };

}

#endif  // SPACETYPER_TEXTURECACHE_H
