#ifndef SPACETYPER_SCALABLESPRITE_H
#define SPACETYPER_SCALABLESPRITE_H

#include <memory>
#include <vector>
#include "core/rect.h"

class Buffer2d;
class Texture2d;
class TextureId;
class TextureCache;
class SpriteRenderer;
class Rgba;

namespace vfs
{
class FileSystem;
}

class ScalableSprite
{
 public:
  ScalableSprite(
      vfs::FileSystem*        fs,
      const std::string& path,
      const Sizef&       size,
      TextureCache*      cache);
  ~ScalableSprite();

  const Sizef
  GetMinimumSize() const;

  void
  Render(SpriteRenderer* sr, const Rectf& pos, const Rgba& tint) const;

 private:
  std::shared_ptr<Texture2d> texture_;

  std::vector<float> cols_;
  std::vector<float> rows_;

  float max_col_;
  float max_row_;
};

#endif  // SPACETYPER_SCALABLESPRITE_H
