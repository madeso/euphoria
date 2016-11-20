#ifndef SPACETYPER_ENEMYWORD_H
#define SPACETYPER_ENEMYWORD_H

#include <random>

#include "spacetyper/sprite.h"
#include "spacetyper/fonts.h"

#include "glm/vec2.hpp"

class TextureCache;
class TextBackgroundRenderer;

class EnemyWord {
 public:
  EnemyWord(TextureCache* cache, Font* font, TextBackgroundRenderer* text_back, const std::string& word);
  ~EnemyWord();

  void Setup(std::mt19937* generator, float screen_width);

  void Update(float delta);

  void AddSprite(Layer* layer);
  void RemoveSprite();

  void Render();


 private:
  Sprite sprite_;
  Text text_;
  glm::vec2 position_;
  Layer* layer_;
};

#endif  // SPACETYPER_ENEMYWORD_H
