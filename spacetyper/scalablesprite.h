#ifndef SPACETYPER_SCALABLESPRITE_H
#define SPACETYPER_SCALABLESPRITE_H

#include <memory>
#include "spacetyper/vec2.h"

class Vao;
class Texture2d;
class TextureCache;

class ScalableSpriteData;

class ScalableSprite {
 public:
  enum class SizeType {CLIENT, WORLD};

  ScalableSprite(Texture2d* texture, const vec2f& min_size, SizeType size_type, const vec2f& size);
  virtual ~ScalableSprite();

  void SetClientSize(const vec2f& new_size);
  const vec2f GetClientSize() const;
  const vec2f GetClientOffset() const;

  void SetSize(const vec2f& new_size);
  const vec2f GetSize() const;

  const vec2f GetMinimumSize() const;
  const vec2f GetMinimumSize(const vec2f& extra) const;



  // for rendering...
  const Texture2d* texture_ptr() const;
  const Vao* vao_ptr() const;

 protected:
  std::pair<SizeType, vec2f> GetSizeData() const;

 private:
  virtual ScalableSpriteData* BuildData() const = 0;

  void PrepareData() const;

  void SetSizeSub(const vec2f& new_size, SizeType type);
  Texture2d* texture_;
  vec2f min_size_;
  SizeType size_type_;
  vec2f size_;

  mutable std::unique_ptr<ScalableSpriteData> data_; // empty = dirty
};

std::shared_ptr<ScalableSprite> LoadScalableSprite(const std::string& path, const vec2f& size, TextureCache* cache);

#endif  // SPACETYPER_SCALABLESPRITE_H
