#ifndef SPACETYPER_SCALABLESPRITE_H
#define SPACETYPER_SCALABLESPRITE_H

#include <memory>
#include <vector>
#include "core/size.h"

class Buffer2d;
class Texture2d;
class TextureCache;

class ScalableSprite {
 public:
  ScalableSprite(const std::string& path, const Sizef& size, TextureCache* cache);
  ~ScalableSprite();

  void SetSize(const Sizef& new_size);
  const Sizef GetSize() const;

  const Sizef GetMinimumSize() const;

  // for rendering...
  const Texture2d* texture_ptr() const;
  const Buffer2d* buffer_ptr() const;



 private:
  void BuildData() const;

  std::shared_ptr<Texture2d> texture_;
  Sizef size_;

  mutable bool dirty_;
  mutable std::unique_ptr<Buffer2d> buffer_;

  std::vector<float> cols_;
  std::vector<float> rows_;

  float max_col_;
  float max_row_;
};

#endif  // SPACETYPER_SCALABLESPRITE_H
