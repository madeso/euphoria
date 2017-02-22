#include "spacetyper/spritefader.h"

#include "core/assert.h"
#include <algorithm>

#include "render/sprite.h"

SpriteFader::SpriteFader(Layer* layer)
    : generator_(std::random_device()())
    , layer_(layer)
{
  assert(layer);
}

void SpriteFader::RegisterTexture(Texture2d *t) {
  assert(this);
  textures_.push_back(t);
}

void SpriteFader::AddRandom(const vec2f& pos, float time, float width, float height) {
  assert(this);
  assert(time > 0.0f);
  assert(!textures_.empty());

  const float dx = std::uniform_real_distribution<float>(-width/2, width/2)(generator_);
  const float dy = std::uniform_real_distribution<float>(-height/2, height/2)(generator_);

  Texture2d* t = textures_[ std::uniform_int_distribution<size_t>(0, textures_.size()-1)(generator_) ];

  FadingSprite sp;
  sp.time = time;
  sp.start = time / 2;
  sp.sprite.reset( new Sprite(t, pos + vec2f(dx, dy)) );

  layer_->Add(sp.sprite.get());
  sprites_.push_back(sp);
}

void SpriteFader::Update(float dt){
  assert(this);

  for(FadingSprite& sp : sprites_) {
    sp.time -= dt;
    const float a = sp.time/sp.start;
    sp.sprite->SetAlpha( std::max(0.0f, std::min(a, 1.0f)) );
    if( sp.time <= 0.0f ) {
      layer_->Remove(sp.sprite.get());
    }
  }

  sprites_.erase(std::remove_if(sprites_.begin(), sprites_.end(), [](const FadingSprite& sp){ return sp.time<=0.0f; } ),
                 sprites_.end());
}
