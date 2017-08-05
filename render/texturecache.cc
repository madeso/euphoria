#include "render/texturecache.h"

#include "core/assert.h"
#include "render/texture.h"

TextureCache::TextureCache(FileSystem* fs) : filesystem_(fs) {
}

TextureCache::~TextureCache() {
}

Texture2d* TextureCache::GetTexture(const std::string& path) {
  auto it = map_.find(path);
  if( it != map_.end()) return it->second.get();

  Ptr t( new Texture2d {} );
  t->LoadFromFile(filesystem_, path, AlphaLoad::Keep, Texture2dLoadData{});
  map_.insert(Map::value_type(path, t));
  return t.get();
}
