#ifndef SPACETYPER_SPRITE_H
#define SPACETYPER_SPRITE_H

#include <vector>
#include <map>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class Texture2d;
class SpriteRenderer;

class Sprite {
 public:
  explicit Sprite(Texture2d* texture, const glm::vec2& position = glm::vec2(0.0f));

  Texture2d* GetTexture();

  const glm::vec2& GetPosition() const;
  void SetPosition(const glm::vec2& p);

  void SetRotation(float r);

  float GetHeight() const;
  float GetWidth() const;

  void Render(SpriteRenderer* render);
 private:
  Texture2d* texture_;
  glm::vec2 position_;
  float rotate_;
  glm::vec2 scale_;
  glm::vec3 color_;
};

class Layer {
 public:
  explicit Layer(SpriteRenderer* render);
  void Add(Sprite* sprite);
  void Remove(Sprite* sprite);
  void Render();
 private:
  SpriteRenderer* render_;
  typedef std::vector<Sprite*> SpriteList;
  typedef std::map<Texture2d*, SpriteList> SpriteMap;
  SpriteMap sprites_;
};

#endif  // SPACETYPER_SPRITE_H