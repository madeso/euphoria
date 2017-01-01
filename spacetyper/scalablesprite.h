#ifndef SPACETYPER_SCALABLESPRITE_H
#define SPACETYPER_SCALABLESPRITE_H

#include <memory>
#include <vector>
#include "core/vec2.h"

class Vao;
class Texture2d;
class TextureCache;

class ScalableSpriteData;

class ScalableSprite {
 public:
  ScalableSprite(const std::string& path, const vec2f& size, TextureCache* cache);
  ~ScalableSprite();

  void SetSize(const vec2f& new_size);
  const vec2f GetSize() const;

  // for rendering...
  const Texture2d* texture_ptr() const;
  const Vao* vao_ptr() const;

 private:
  void BuildData() const;

  Texture2d* texture_;
  vec2f size_;

  mutable bool dirty_;
  mutable std::unique_ptr<Vao> vao_;

  std::vector<float> cols_;
  std::vector<float> rows_;

  float max_col_;
  float max_row_;
};

#endif  // SPACETYPER_SCALABLESPRITE_H
