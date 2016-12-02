#ifndef SPACETYPER_SCALABLESPRITE_H
#define SPACETYPER_SCALABLESPRITE_H

#include <memory>
#include "glm/vec2.hpp"

class Vao;
class Texture2d;
class TextureCache;

class ScalableSpriteData;

class ScalableSprite {
 public:
  enum class SizeType {CLIENT, WORLD};

  ScalableSprite(Texture2d* texture, const glm::vec2& min_size, SizeType size_type, const glm::vec2& size);
  virtual ~ScalableSprite();

  void SetClientSize(const glm::vec2& new_size);
  const glm::vec2 GetClientSize() const;
  const glm::vec2 GetClientOffset() const;

  void SetSize(const glm::vec2& new_size);
  const glm::vec2 GetSize() const;

  const glm::vec2 GetMinimumSize() const;
  const glm::vec2 GetMinimumSize(const glm::vec2& extra) const;



  // for rendering...
  const Texture2d* texture_ptr() const;
  const Vao* vao_ptr() const;

 protected:
  std::pair<SizeType, glm::vec2> GetSizeData() const;

 private:
  virtual ScalableSpriteData* BuildData() const = 0;

  void PrepareData() const;

  void SetSizeSub(const glm::vec2& new_size, SizeType type);
  Texture2d* texture_;
  glm::vec2 min_size_;
  SizeType size_type_;
  glm::vec2 size_;

  mutable std::unique_ptr<ScalableSpriteData> data_; // empty = dirty
};

std::shared_ptr<ScalableSprite> LoadScalableSprite(const std::string& path, const glm::vec2& size, TextureCache* cache);

#endif  // SPACETYPER_SCALABLESPRITE_H
