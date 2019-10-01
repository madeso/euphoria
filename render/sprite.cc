#include "render/sprite.h"

#include <algorithm>

#include "core/vec4.h"

#include "render/spriterender.h"

namespace euphoria::render
{

  Sprite::Sprite(std::shared_ptr<Texture2d> texture, const core::vec2f& position)
      : texture_(texture)
      , position_(position)
      , rotation(0.0_rad)
      , scale_(core::scale2f(1, 1))
      , color_(core::Rgb(1.0f))
      , alpha_(1.0f)
  {
  }

  std::shared_ptr<Texture2d>
  Sprite::GetTexture()
  {
    return texture_;
  }

  const core::vec2f&
  Sprite::GetPosition() const
  {
    return position_;
  }

  void
  Sprite::SetPosition(const core::vec2f& p)
  {
    position_ = p;
  }

  float
  Sprite::GetHeight() const
  {
    return texture_->GetHeight() * scale_.y;
  }

  float
  Sprite::GetWidth() const
  {
    return texture_->GetWidth() * scale_.x;
  }

  void
  Sprite::SetAlpha(float a)
  {
    alpha_ = a;
  }


  void
  Sprite::Render(SpriteRenderer* render)
  {
    render->DrawSprite(
        *texture_,
        core::Rectf::FromPositionAnchorWidthAndHeight(
            position_,
            core::scale2f{0, 0},
            texture_->GetWidth(),
            texture_->GetHeight()),
        DrawData{}.Rotation(rotation).Scale(scale_).Tint(core::Rgba(color_, alpha_)));
  }

  Layer::Layer(SpriteRenderer* render)
      : render_(render)
  {
  }

  void
  Layer::Add(Sprite* sprite)
  {
    sprites_[sprite->GetTexture()].push_back(sprite);
  }

  void
  Layer::Remove(Sprite* sprite)
  {
    SpriteList& sprites = sprites_[sprite->GetTexture()];
    sprites.erase(
        std::remove(sprites.begin(), sprites.end(), sprite), sprites.end());
  }

  void
  Layer::Render()
  {
    for(auto& list : sprites_)
    {
      for(auto& sp : list.second)
      {
        sp->Render(render_);
      }
    }
  }

}
