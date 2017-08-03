#ifndef SPACETYPER_TEXTURECACHE_H
#define SPACETYPER_TEXTURECACHE_H

#include <memory>
#include <string>
#include <map>

class Texture2d;
class FileSystem;

class TextureCache {
 public:
  explicit TextureCache(FileSystem* fs);
  ~TextureCache();

  Texture2d* GetTexture(const std::string& path);

 private:
  FileSystem* filesystem_;
  typedef std::shared_ptr<Texture2d> Ptr;
  typedef std::map<std::string, Ptr> Map;
  Map map_;
};

#endif  // SPACETYPER_TEXTURECACHE_H
