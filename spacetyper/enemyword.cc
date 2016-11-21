#include "spacetyper/enemyword.h"

#include <cassert>
#include <iostream>

#include "spacetyper/texturecache.h"

EnemyWord::EnemyWord(TextureCache* cache, Font* font, TextBackgroundRenderer* text_back, const std::string& word)
    : sprite_(cache->GetTexture("enemyShip.png"))
    , text_(font, text_back)
    , position_(0.0f)
    , layer_(nullptr)
    , speed_(0.0f)
    , index_(0)
{
  text_.SetText(word);
  text_.SetAlignment(Align::CENTER);
  text_.SetBackground(true, 0.8f);
  text_.SetBaseColor(glm::vec3(1.0f));
  text_.SetHighlightColor(glm::vec3(0.0f, 0.0f, 1.0f));
}

EnemyWord::~EnemyWord() {
  RemoveSprite();
}

void EnemyWord::Setup(std::mt19937* generator, float screen_width) {
  assert(this);
  assert(generator);

  const float w = std::max(sprite_.GetWidth(), text_.GetExtents().GetWidth());
  const float x = std::uniform_real_distribution<float>(w/2.0f, screen_width - w/2.0f)(*generator);
  const float neg_y = sprite_.GetHeight() / 2.0f;

  speed_ = std::uniform_real_distribution<float>(20.0f, 40.0f)(*generator);
  position_.x = x;
  position_.y = -neg_y;
}

void EnemyWord::Update(float delta) {
  assert(this);
  position_.y += delta * speed_;
  sprite_.SetPosition(position_);
}

void EnemyWord::AddSprite(Layer* layer) {
  assert(this);
  assert(layer_ == nullptr);
  layer_ = layer;
  layer_->Add(&sprite_);
}

void EnemyWord::RemoveSprite() {
  assert(this);
  if( layer_ != nullptr ) {
    layer_->Remove(&sprite_);
    layer_ = nullptr;
  }
}

void EnemyWord::Render() {
  glm::vec2 p = position_;
  p.y += sprite_.GetHeight() / 2.0f;
  text_.Draw(p);
}

bool EnemyWord::Type(const std::string& input) {
  assert(IsAlive());
  const std::string& t = text_.GetText();
  const char c = t[index_];
  const std::string cstr(1, c);
  const bool is_same = cstr == input;

  if( is_same ) {
    index_ += 1;
    text_.SetHighlightRange(0, index_);
  }

  return is_same;
}

bool EnemyWord::IsAlive() const {
  const std::string& t = text_.GetText();
  return index_ < t.length();
}

const std::string& EnemyWord::GetWord() const {
  return text_.GetText();
}
