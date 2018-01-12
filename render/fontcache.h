#ifndef RENDER_FONTCACHE_H
#define RENDER_FONTCACHE_H

#include <memory>
#include <string>

class Font;
class FileSystem;

class FontCache
{
 public:
  explicit FontCache(FileSystem* fs);
  ~FontCache();

  std::shared_ptr<Font>
  GetFont(const std::string& path);

 private:
  struct FontCachePimpl;
  std::unique_ptr<FontCachePimpl> pimp;
};


#endif  // RENDER_FONTCACHE_H
