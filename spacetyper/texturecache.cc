#include "spacetyper/texturecache.h"

#include <cassert>
#include "spacetyper/texture.h"

TextureCache::TextureCache() {
}

TextureCache::~TextureCache() {
}

Texture2d* TextureCache::GetTexture(const std::string& path) {
  auto it = map_.find(path);
  if( it != map_.end()) return it->second.get();

  Ptr t( new Texture2d(path) );
  map_.insert(Map::value_type(path, t));
  return t.get();
}
