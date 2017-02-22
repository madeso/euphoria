#include "spacetyper/enemyword.h"

#include "core/assert.h"
#include <iostream>

#include "render/texturecache.h"
#include "spacetyper/spritefader.h"

const int max_explosions = 20;

EnemyWord::EnemyWord(SpriteFader* fader, TextureCache* cache, Font* font, TextBackgroundRenderer* text_back, const std::string& word)
    : fader_(fader)
    , sprite_(cache->GetTexture("enemyShip.png"))
    , text_(font, text_back)
    , position_(0.0f)
    , layer_(nullptr)
    , speed_(0.0f)
    , index_(0)
    , health_(word.length())
    , explisiontimer_(0.0f)
    , explosions_(0)
    , knockback_(-1.0f)
{
  text_.SetText(word);
  text_.SetAlignment(Align::CENTER);
  text_.SetBackground(true, 0.8f);
  text_.SetBaseColor(Rgb(1.0f));
  text_.SetHighlightColor(Rgb(0.0f, 0.0f, 1.0f));
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

  const float speed = knockback_<=0.0f? speed_
                      : speed_ * (1.0f - knockback_*2.0f);
  if( knockback_ > 0.0f ) {
    knockback_ -= delta * 5.0f;
  }

  position_.y += delta * speed;
  sprite_.SetPosition(position_);

  if( health_ <= 0 ) {
    const float a = 1.0f - static_cast<float>(explosions_)/max_explosions;
    sprite_.SetAlpha(a);
    explisiontimer_ -= delta;
    while( explisiontimer_ < 0.0f) {
      float scale = 0.8f;
      fader_->AddRandom(GetPosition(), 0.2f, sprite_.GetWidth() * scale, sprite_.GetHeight() * scale);
      ++explosions_;
      explisiontimer_ += 0.05f;
    }
  }
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
  assert(this);
  vec2f p = position_;
  p.y += sprite_.GetHeight() / 2.0f;
  text_.Draw(p);
}

bool EnemyWord::Type(const std::string& input) {
  assert(this);
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
  assert(this);
  const std::string& t = text_.GetText();
  return index_ < t.length();
}

const std::string& EnemyWord::GetWord() const {
  assert(this);
  return text_.GetText();
}

const vec2f& EnemyWord::GetPosition() const {
  assert(this);
  return sprite_.GetPosition();
}

const Sizef EnemyWord::GetSize() const {
  assert(this);
  return Sizef::FromHeightWidth(sprite_.GetWidth(), sprite_.GetHeight());
}

void EnemyWord::Damage() {
  assert(this);
  health_ -= 1;

  if( health_ <= 0 ) {
    // speed_ = speed_ / 2.0f;
  }

  knockback_ += 0.3f;
  knockback_ = std::max(knockback_, 1.0f);

  const float scale = 0.8f;
  for(int i=0; i<4; ++i) {
    fader_->AddRandom(GetPosition(), 0.2f, sprite_.GetWidth() * scale, sprite_.GetHeight() * scale);
  }
}

bool EnemyWord::IsDestroyed() const {
  assert(this);
  return health_ <= 0 && explosions_ > max_explosions;
}
