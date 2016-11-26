#include "spacetyper/bulletlist.h"

#include <cassert>
#include <algorithm>

#include "spacetyper/sprite.h"
#include "spacetyper/enemyword.h"

#include "glm/glm.hpp"

// todo: improve resolution
const float PI = 3.14;

BulletType::~BulletType() {}

BulletList::BulletList(Layer* layer) : layer_(layer) {
  assert(layer);
}

float BulletList::Add(EnemyWord* word, Texture2d* t, const glm::vec2& pos) {
  assert(this);
  BulletType b;
  b.word = word;
  b.sprite.reset( new Sprite(t, pos) );
  layer_->Add(b.sprite.get());
  bullets_.push_back(b);

  const glm::vec2& p = pos;
  const glm::vec2& w = word->GetPosition();
  const glm::vec2 d = w-p;
  const glm::vec2 dn = glm::normalize(d);
  float aa = glm::dot(glm::vec2(0.0f, 1.0f), dn);
  float a = PI - acos(aa);
  if( d.x < 0 ) a = -a;
  return a;
}

void BulletList::Update(float dt) {
  assert(this);
  const float speed = 1000.0f;
  for(BulletType& b : bullets_) {
    const glm::vec2& p = b.sprite->GetPosition();
    const glm::vec2& w = b.word->GetPosition();

    const glm::vec2 d = w-p;
    if( glm::length(d) < speed*dt ) {
      b.word->Damage();
      b.word = nullptr;
      layer_->Remove(b.sprite.get());
    }
    else {
      const glm::vec2 dn = glm::normalize(d);
      b.sprite->SetPosition(p + dn * speed*dt);

      float aa = glm::dot(glm::vec2(0.0f, 1.0f), dn);
      float a = PI - acos(aa);
      if( dn.x < 0 ) a = -a;
      b.sprite->SetRotation(a);
    }
  }

  bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const BulletType& b){return b.word == nullptr;}),
                 bullets_.end());
}
