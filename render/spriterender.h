#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <memory>

#include "core/vec2.h"
#include "core/rgb.h"
#include "core/angle.h"
#include "core/rect.h"

#include "render/shaderuniform.h"
#include "render/shader.h"
#include "render/texture.h"

class ScalableSprite;
class Buffer2d;

struct DrawData
{
  Angle rotation = Angle::Zero();
  vec2f scale    = vec2f(1, 1);
  Rgba  tint     = Rgba(Color::White);

  DrawData&
  Rotation(const Angle& r);

  DrawData&
  Scale(const vec2f& s);

  DrawData&
  Tint(const Rgba& t);
};

class SpriteRenderer
{
 public:
  SpriteRenderer(Shader* shader);
  ~SpriteRenderer();

  void
  DrawRect(
      const Texture2d& texture,
      const Rectf&     sprite_area,
      const Rectf&     texture_region,
      const Angle&     rotation_angle,
      const vec2f&     rotation_anchor,
      const Rgba&      tint_color);

  // position is center
  void
  DrawSprite(
      const Texture2d& texture,
      const Rectf&     position,
      const DrawData&  data = DrawData{});

  // bottom left
  void
  DrawNinepatch(
      const ScalableSprite& ninepatch, const Rectf& rect, const Rgba& tint);

 private:
  Shader*                   shader_;
  ShaderUniform             color_;
  ShaderUniform             model_;
  ShaderUniform             texture_area_;
  std::unique_ptr<Buffer2d> buffer_;

  void
  InitRenderData();
};

#endif
