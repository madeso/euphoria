#ifndef SPACETYPER_ENEMIES_H
#define SPACETYPER_ENEMIES_H

#include <vector>
#include <memory>
#include <random>

class EnemyWord;
class TextureCache;
class Font;
class Layer;

class Enemies {
 public:
  Enemies(TextureCache* cache, Font* font, Layer* layer, float width);
  ~Enemies();

  void SpawnEnemies(int count);

  void AddEnemy();
  int EnemyCount();

  void Update(float delta);
  void Render();

 private:
  mutable std::mt19937 generator_;
  TextureCache* cache_;
  Font* font_;
  Layer* layer_;
  float width_;

  int spawn_count_;
  float spawn_time_;


  typedef std::shared_ptr<EnemyWord> EnemyPtr;
  typedef std::vector<EnemyPtr> EnemyList;
  EnemyList enemies_;
};

#endif  // SPACETYPER_ENEMIES_H