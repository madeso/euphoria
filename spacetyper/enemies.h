#ifndef SPACETYPER_ENEMIES_H
#define SPACETYPER_ENEMIES_H

#include <vector>
#include <memory>
#include <random>
#include <string>

#include "glm/vec2.hpp"

class EnemyWord;
class TextureCache;
class Font;
class TextBackgroundRenderer;
class Layer;
class Dictionary;
class Sprite;
class BulletList;

class Enemies {
 public:
  Enemies(TextureCache* cache, Font* font, TextBackgroundRenderer* text_back, Layer* layer, Dictionary* dictionary, float width, BulletList* bullets);
  ~Enemies();

  void SpawnEnemies(int count);

  void AddEnemy();
  int EnemyCount();

  void Update(float delta);
  void Render();

  EnemyWord* DetectWord(const std::string& input);
  void Remove(EnemyWord* word);

  void FireAt(const glm::vec2& pos, EnemyWord* word);

 private:
  mutable std::mt19937 generator_;
  TextureCache* cache_;
  Font* font_;
  TextBackgroundRenderer* text_back_;
  Layer* layer_;
  Dictionary* dictionary_;
  float width_;

  int spawn_count_;
  float spawn_time_;


  typedef std::shared_ptr<EnemyWord> EnemyPtr;
  typedef std::vector<EnemyPtr> EnemyList;
  EnemyList enemies_;
  EnemyList destroyed_;

  BulletList* bullets_;
};

#endif  // SPACETYPER_ENEMIES_H