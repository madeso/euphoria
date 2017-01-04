#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <memory>
#include "render/shader.h"
#include "render/texture.h"

#include "core/vec2.h"
#include "core/vec4.h"

class Vao;
class ScalableSprite;

class SpriteRenderer {
 public:
  SpriteRenderer(Shader* shader);
  ~SpriteRenderer();

  void DrawSprite(const Texture2d& texture, const vec2f& position,
                  float rotate = 0.0f,
                  const vec2f& scale = vec2f(1, 1),
                  const vec4f& color = vec4f(1.0f));
  void DrawNinepatch(const ScalableSprite& ninepatch, const vec2f& position,
                  float rotate = 0.0f,
                  const vec2f& scale = vec2f(1, 1),
                  const vec4f& color = vec4f(1.0f));

 private:
  Shader* shader_;
  std::unique_ptr<Vao> vao_;

  void InitRenderData();

  void CommonDraw(const vec2f& position, float rotate, const vec4f &color,
             const vec2f &size, const vec2f &scale) const;
};

#endif