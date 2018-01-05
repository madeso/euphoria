#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <memory>

#include "core/vec2.h"
#include "core/rgb.h"
#include "core/angle.h"

#include "render/shaderuniform.h"
#include "render/shader.h"
#include "render/texture.h"

class ScalableSprite;
class Buffer2d;

struct DrawData
{
  Angle rotation = Angle::Zero();
  vec2f scale    = vec2f(1, 1);
  Rgba  tint     = Rgba(1.0f);

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

  // position is center
  void
  DrawSprite(
      const Texture2d& texture,
      const vec2f&     position,
      const DrawData&  data = DrawData{});

  // bottom left
  void
  DrawNinepatch(
      const ScalableSprite& ninepatch,
      const vec2f&          position,
      const DrawData&       data = DrawData{});

 private:
  Shader*                   shader_;
  ShaderUniform             color_;
  ShaderUniform             model_;
  std::unique_ptr<Buffer2d> buffer_;

  void
  InitRenderData();

  void
  CommonDraw(const vec2f& position, const DrawData& data) const;
};

#endif
