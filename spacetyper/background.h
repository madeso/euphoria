#ifndef SPACETYPER_BACKGROUND_H
#define SPACETYPER_BACKGROUND_H

#include <vector>
#include "glm/vec2.hpp"

class Texture2d;
class SpriteRenderer;

class Background {
 public:
  Background(int count, int width, int height, Texture2d* texture, float speed);

  void Update(float delta);
  void Render(SpriteRenderer* renderer) const;

 private:
  int width_;
  int height_;
  float speed_;
  Texture2d* texture_;
  std::vector<glm::vec2> positions_;
};

#endif  // SPACETYPER_BACKGROUND_H
