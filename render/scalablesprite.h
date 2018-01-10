#ifndef SPACETYPER_SCALABLESPRITE_H
#define SPACETYPER_SCALABLESPRITE_H

#include <memory>
#include <vector>
#include "core/size.h"

class Buffer2d;
class Texture2d;
class TextureId;
class TextureCache;
class SpriteRenderer;

class ScalableSprite
{
 public:
  ScalableSprite(
      const std::string& path, const Sizef& size, TextureCache* cache);
  ~ScalableSprite();

  void
  SetSize(const Sizef& new_size);

  const Sizef
  GetSize() const;

  const Sizef
  GetMinimumSize() const;

  void
  Render(SpriteRenderer* sr, const vec2f& pos) const;

 private:
  std::shared_ptr<Texture2d> texture_;
  Sizef                      size_;

  std::vector<float> cols_;
  std::vector<float> rows_;

  float max_col_;
  float max_row_;
};

#endif  // SPACETYPER_SCALABLESPRITE_H
