#ifndef SPACETYPER_SPRITEFADER_H
#define SPACETYPER_SPRITEFADER_H

#include <memory>
#include <vector>
#include <random>

#include "spacetyper/vec2.h"

class Texture2d;
class Layer;
class Sprite;

struct FadingSprite {
  float time;
  float start;
  std::shared_ptr<Sprite> sprite;
};

class SpriteFader {
 public:
  explicit SpriteFader(Layer* layer);
  void RegesterTexture(Texture2d* t);
  void AddRandom(const vec2f& pos, float time, float width, float height);

  void Update(float dt);

 private:
  mutable std::mt19937 generator_;
  Layer* layer_;

  typedef std::vector<Texture2d*> Textures;
  Textures textures_;

  typedef std::vector<FadingSprite> Sprites;
  Sprites sprites_;
};

#endif  // SPACETYPER_SPRITEFADER_H