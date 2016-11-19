#include "spacetyper/enemies.h"

#include <cassert>

#include "spacetyper/enemyword.h"

Enemies::Enemies(TextureCache* cache, Font* font, Layer* layer, float width) : generator_(std::random_device()()), cache_(cache), font_(font), layer_(layer), width_(width) {
  assert(cache);
  assert(font);
  assert(layer);
}

Enemies::~Enemies() {
}

void Enemies::AddEnemy() {
  assert(this);

  // todo: use random words
  EnemyPtr e(new EnemyWord(cache_, font_, "dog"));
  e->AddSprite(layer_);
  e->Setup(&generator_, width_);
  enemies_.push_back(e);
}

int Enemies::EnemyCount() {
  assert(this);
  return enemies_.size();
}

void Enemies::Update(float delta) {
  assert(this);
  for(auto& e : enemies_) {
    e->Update(delta);
  }
}

void Enemies::Render() {
  assert(this);
  for(auto& e : enemies_) {
    e->Render();
  }
}
