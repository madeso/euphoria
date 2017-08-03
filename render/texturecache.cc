#include "render/texturecache.h"

#include "core/assert.h"
#include "render/texture.h"

TextureCache::TextureCache() {
}

TextureCache::~TextureCache() {
}

Texture2d* TextureCache::GetTexture(const std::string& path) {
  auto it = map_.find(path);
  if( it != map_.end()) return it->second.get();

  Ptr t( new Texture2d {} );
  t->LoadFromFile(path, AlphaLoad::Include, Texture2dLoadData{});
  map_.insert(Map::value_type(path, t));
  return t.get();
}
