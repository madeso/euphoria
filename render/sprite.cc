#include "render/sprite.h"

#include "core/assert.h"
#include <algorithm>

#include "core/vec4.h"

#include "render/spriterender.h"

Sprite::Sprite(Texture2d* texture, const vec2f& position)
    : texture_(texture)
    , position_(position)
    , rotate_(0.0f)
    , scale_(vec2f(1, 1))
    , color_(Rgb(1.0f))
    , alpha_(1.0f)
{ }

Texture2d* Sprite::GetTexture() {
  Assert(this);
  return texture_;
}

const vec2f& Sprite::GetPosition() const {
  Assert(this);
  return position_;
}

void Sprite::SetPosition(const vec2f& p) {
  Assert(this);
  position_ = p;
}

void Sprite::SetRotation(float r) {
  Assert(this);
  rotate_ = r;
}

float Sprite::GetRotation() const {
  Assert(this);
  return rotate_;
}

float Sprite::GetHeight() const {
  Assert(this);
  return texture_->height() * scale_.y;
}

float Sprite::GetWidth() const {
  Assert(this);
  return texture_->width() * scale_.x;
}

void Sprite::SetAlpha(float a) {
  Assert(this);
  alpha_ = a;
}


void Sprite::Render(SpriteRenderer* render) {
  Assert(this);
  render->DrawSprite(*texture_, position_, rotate_, scale_, Rgba(color_, alpha_));
}

Layer::Layer(SpriteRenderer* render) : render_(render) {
}

void Layer::Add(Sprite* sprite) {
  Assert(this);
  sprites_[sprite->GetTexture()].push_back(sprite);
}

void Layer::Remove(Sprite* sprite) {
  Assert(this);
  SpriteList& sprites = sprites_[sprite->GetTexture()];
  sprites.erase(std::remove(sprites.begin(), sprites.end(), sprite),
                sprites.end());
}

void Layer::Render() {
  Assert(this);
  for (auto& list : sprites_) {
    for(auto& sp: list.second) {
      sp->Render(render_);
    }
  }
}
