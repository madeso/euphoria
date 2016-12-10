#ifndef SPACETYPER_ENEMYWORD_H
#define SPACETYPER_ENEMYWORD_H

#include <random>

#include "spacetyper/sprite.h"
#include "spacetyper/fonts.h"

#include "spacetyper/vec2.h"

class TextureCache;
class TextBackgroundRenderer;
class SpriteFader;

class EnemyWord {
 public:
  EnemyWord(SpriteFader* fader, TextureCache* cache, Font* font, TextBackgroundRenderer* text_back, const std::string& word);
  ~EnemyWord();

  void Setup(std::mt19937* generator, float screen_width);

  void Update(float delta);

  void AddSprite(Layer* layer);
  void RemoveSprite();

  void Render();

  bool Type(const std::string& input);
  bool IsAlive() const;

  const std::string& GetWord() const;
  const vec2f& GetPosition() const;
  const vec2f GetSize() const;

  void Damage();
  bool IsDestroyed() const;

 private:
  SpriteFader* fader_;
  Sprite sprite_;
  Text text_;
  vec2f position_;
  Layer* layer_;
  float speed_;
  unsigned int index_;
  int health_;
  float explisiontimer_;
  int explosions_;
  float knockback_;
};

#endif  // SPACETYPER_ENEMYWORD_H
