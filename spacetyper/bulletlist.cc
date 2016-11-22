#include "spacetyper/bulletlist.h"

#include <cassert>
#include <algorithm>
#include <iostream>

#include "spacetyper/sprite.h"
#include "spacetyper/enemyword.h"

#include "glm/glm.hpp"

BulletType::~BulletType() {}

BulletList::BulletList(Layer* layer) : layer_(layer) {
  assert(layer);
}

void BulletList::Add(EnemyWord* word, Texture2d* t, const glm::vec2& pos) {
  assert(this);
  BulletType b;
  b.word = word;
  b.sprite.reset( new Sprite(t, pos) );
  layer_->Add(b.sprite.get());
  bullets_.push_back(b);

  std::cout << "spawn bullet\n";
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
      b.sprite->SetPosition(p + glm::normalize(d) * speed*dt);
    }
  }

  bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const BulletType& b){return b.word == nullptr;}),
                 bullets_.end());
}
