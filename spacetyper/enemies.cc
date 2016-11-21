#include "spacetyper/enemies.h"

#include <cassert>
#include <algorithm>

#include "spacetyper/enemyword.h"
#include "spacetyper/dictionary.h"

Enemies::Enemies(TextureCache* cache, Font* font, TextBackgroundRenderer* text_back, Layer* layer, Dictionary* dictionary, float width)
    : generator_(std::random_device()())
    , cache_(cache)
    , font_(font)
    , text_back_(text_back)
    , layer_(layer)
    , dictionary_(dictionary)
    , width_(width)
    , spawn_count_(0)
    , spawn_time_(-1.0f)
{
  assert(cache);
  assert(font);
  assert(layer);
}

Enemies::~Enemies() {
}

void Enemies::SpawnEnemies(int count) {
  assert(this);
  spawn_count_ += count;
}

void Enemies::AddEnemy() {
  assert(this);

  EnemyPtr e(new EnemyWord(cache_, font_, text_back_, dictionary_->Generate()));
  e->AddSprite(layer_);
  e->Setup(&generator_, width_);
  e->Update(0.0f);
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

  if( spawn_count_ > 0 ) {
    spawn_time_ -= delta;
    if( spawn_time_ < 0) {
      AddEnemy();
      spawn_count_ -= 1;
      if( spawn_count_ > 0 ) {
        spawn_time_ += 1;
      }
    }
  }
}

void Enemies::Render() {
  assert(this);
  for(auto& e : enemies_) {
    e->Render();
  }
}

EnemyWord* Enemies::DetectWord(const std::string& input) {
  assert(this);
  for(auto& e : enemies_) {
    if( e->Type(input) ) {
      return e.get();
    }
  }

  return nullptr;
}

void Enemies::Remove(EnemyWord* word) {
  assert(this);
  assert(word);
  enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [&word](EnemyPtr rhs){return rhs.get() == word; }),
                enemies_.end());
}
