#include "spacetyper/sprite.h"

#include <cassert>
#include <algorithm>

#include "spacetyper/spriterender.h"

Sprite::Sprite(Texture2d* texture, const glm::vec2& position)
    : texture_(texture)
    , position_(position)
    , rotate_(0.0f)
    , scale_(glm::vec2(1, 1))
    , color_(glm::vec3(1.0f))
{ }

Texture2d* Sprite::GetTexture() {
  assert(this);
  return texture_;
}

const glm::vec2& Sprite::GetPosition() const {
  assert(this);
  return position_;
}

void Sprite::SetPosition(const glm::vec2& p) {
  assert(this);
  position_ = p;
}

void Sprite::SetRotation(float r) {
  assert(this);
  rotate_ = r;
}

float Sprite::GetHeight() const {
  assert(this);
  return texture_->height() * scale_.y;
}

float Sprite::GetWidth() const {
  assert(this);
  return texture_->width() * scale_.x;
}


void Sprite::Render(SpriteRenderer* render) {
  assert(this);
  render->DrawSprite(*texture_, position_, rotate_, scale_, color_);
}

Layer::Layer(SpriteRenderer* render) : render_(render) {
}

void Layer::Add(Sprite* sprite) {
  assert(this);
  sprites_[sprite->GetTexture()].push_back(sprite);
}

void Layer::Remove(Sprite* sprite) {
  assert(this);
  SpriteList& sprites = sprites_[sprite->GetTexture()];
  sprites.erase(std::remove(sprites.begin(), sprites.end(), sprite),
                sprites.end());
}

void Layer::Render() {
  assert(this);
  for (auto& list : sprites_) {
    for(auto& sp: list.second) {
      sp->Render(render_);
    }
  }
}
