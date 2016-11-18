#ifndef SPACETYPER_TEXTURECACHE_H
#define SPACETYPER_TEXTURECACHE_H

#include <memory>
#include <string>
#include <map>

class Texture2d;

class TextureCache {
 public:
  TextureCache();
  ~TextureCache();

  Texture2d* GetTexture(const std::string& path);

 private:
  typedef std::shared_ptr<Texture2d> Ptr;
  typedef std::map<std::string, Ptr> Map;
  Map map_;
};

#endif  // SPACETYPER_TEXTURECACHE_H
