#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <memory>
#include "render/shader.h"
#include "render/texture.h"

#include "core/vec2.h"
#include "core/rgb.h"
#include "render/shaderuniform.h"

class ScalableSprite;

class Buffer2d;

class SpriteRenderer
{
 public:
  SpriteRenderer(Shader* shader);
  ~SpriteRenderer();

  void
  DrawSprite(
      const Texture2d& texture,
      const vec2f&     position,
      float            rotate = 0.0f,
      const vec2f&     scale  = vec2f(1, 1),
      const Rgba&      color  = Rgba(1.0f));
  void
  DrawNinepatch(
      const ScalableSprite& ninepatch,
      const vec2f&          position,
      float                 rotate = 0.0f,
      const vec2f&          scale  = vec2f(1, 1),
      const Rgba&           color  = Rgba(1.0f));

 private:
  Shader*                   shader_;
  ShaderUniform             color_;
  ShaderUniform             model_;
  std::unique_ptr<Buffer2d> buffer_;

  void
  InitRenderData();

  void
  CommonDraw(
      const vec2f& position,
      float        rotate,
      const Rgba&  color,
      const vec2f& size,
      const vec2f& scale) const;
};

#endif
