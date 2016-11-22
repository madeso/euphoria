#ifndef SPACETYPER_BULLETLIST_H
#define SPACETYPER_BULLETLIST_H

#include <vector>
#include <memory>

#include "glm/vec2.hpp"

#include "spacetyper/sprite.h"

class EnemyWord;
class Layer;

class BulletType {
 public:
  typedef std::shared_ptr<Sprite> SpritePtr;

  ~BulletType();

  EnemyWord* word;
  SpritePtr sprite;
};

class BulletList {
 public:
  explicit BulletList(Layer* layer);
  void Add(EnemyWord* word, Texture2d* t, const glm::vec2& pos);
  void Update(float d);
 private:
  Layer* layer_;
  typedef std::vector<BulletType> Bullets;
  Bullets bullets_;
};

#endif  // SPACETYPER_BULLETLIST_H
