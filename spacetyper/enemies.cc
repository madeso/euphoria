#include "spacetyper/enemies.h"

#include <cassert>
#include <algorithm>

#include "spacetyper/enemyword.h"
#include "spacetyper/dictionary.h"
#include "spacetyper/texturecache.h"
#include "spacetyper/bulletlist.h"

Enemies::Enemies(SpriteFader* fader, TextureCache* cache, Font* font, TextBackgroundRenderer* text_back, Layer* layer, Dictionary* dictionary, float width, BulletList* bullets)
    : fader_(fader)
    , generator_(std::random_device()())
    , cache_(cache)
    , font_(font)
    , text_back_(text_back)
    , layer_(layer)
    , dictionary_(dictionary)
    , width_(width)
    , spawn_count_(0)
    , spawn_time_(-1.0f)
    , bullets_(bullets)
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

std::string GenerateUniqueWord(const std::string& start, Dictionary* dict) {
  std::string word = dict->Generate();
  unsigned int loop = 0;
  while( start.find(word[0]) != std::string::npos && loop < 10 ) {
    word = dict->Generate();
    ++loop;
  }
  return word;
}

void Enemies::AddEnemy() {
  assert(this);

  std::string characters;
  for(auto& w:enemies_) {
    characters += w->GetWord()[0];
  }

  EnemyPtr e(new EnemyWord(fader_, cache_, font_, text_back_, GenerateUniqueWord(characters, dictionary_)));
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

  for(auto& e : destroyed_) {
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



  destroyed_.erase(std::remove_if(destroyed_.begin(), destroyed_.end(), [](EnemyPtr enemy){return enemy->IsDestroyed(); }),
                   destroyed_.end());
}

void Enemies::Render() {
  assert(this);
  for(auto& e : enemies_) {
    e->Render();
  }
}

EnemyWord* Enemies::DetectWord(const std::string& input) {
  assert(this);
  for(EnemyList::iterator it = enemies_.begin(); it != enemies_.end(); ++it) {
    EnemyPtr e = *it;
    if( e->Type(input) ) {
      // move to back = move rendering to front
      enemies_.erase(it);
      enemies_.push_back(e);
      return e.get();
    }
  }

  return nullptr;
}

void Enemies::Remove(EnemyWord* word) {
  assert(this);
  assert(word);
  EnemyList::iterator found = std::find_if(enemies_.begin(), enemies_.end(), [&word](EnemyPtr rhs){return rhs.get() == word; });
  assert(found != enemies_.end());
  destroyed_.push_back(*found);
  enemies_.erase(found);
}

float Enemies::FireAt(const vec2f& pos, EnemyWord* word) {
  return bullets_->Add(word, cache_->GetTexture("laserBlue07.png"), pos);
}
