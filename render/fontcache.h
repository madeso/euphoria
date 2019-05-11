#ifndef RENDER_FONTCACHE_H
#define RENDER_FONTCACHE_H

#include <memory>
#include <string>

class Font;
class TextureCache;

namespace vfs
{
class FileSystem;
}

class FontCache
{
 public:
  FontCache(vfs::FileSystem* fs, TextureCache* cache);
  ~FontCache();

  std::shared_ptr<Font>
  GetFont(const std::string& path);

 private:
  struct FontCachePimpl;
  std::unique_ptr<FontCachePimpl> pimp;
};


#endif  // RENDER_FONTCACHE_H
