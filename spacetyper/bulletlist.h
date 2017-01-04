#ifndef SPACETYPER_BULLETLIST_H
#define SPACETYPER_BULLETLIST_H

#include <vector>
#include <memory>

#include "core/vec2.h"

#include "render/sprite.h"

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
  float Add(EnemyWord* word, Texture2d* t, const vec2f& pos);
  void Update(float d);
 private:
  Layer* layer_;
  typedef std::vector<BulletType> Bullets;
  Bullets bullets_;
};

#endif  // SPACETYPER_BULLETLIST_H
