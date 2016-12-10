#ifndef SPACETYPER_BACKGROUND_H
#define SPACETYPER_BACKGROUND_H

#include <vector>
#include "spacetyper/sprite.h"

class Texture2d;
class SpriteRenderer;

class Background {
 public:
  Background(int count, int width, int height, Texture2d* texture, float speed, Layer* layer);

  void Update(float delta);

 private:
  float width_;
  float height_;
  float speed_;
  std::vector<Sprite> positions_;
};

#endif  // SPACETYPER_BACKGROUND_H
