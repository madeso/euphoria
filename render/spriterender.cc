#include "render/spriterender.h"
#include "render/buffer2d.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/gl.h"
#include "render/bufferbuilder2d.h"

SpriteRenderer::SpriteRenderer(Shader* shader)
    : shader_(shader)
    , color_(shader->GetUniform("color"))
    , model_(shader->GetUniform("model"))
{
  shader_ = shader;
  InitRenderData();
}

SpriteRenderer::~SpriteRenderer() { buffer_.reset(); }

void SpriteRenderer::DrawSprite(const Texture2d& texture,
                                const vec2f& position, float rotate,
                                const vec2f& scale,
                                const Rgba& color) {
  const vec2f size(scale.x * texture.width(), scale.y * texture.height());
  CommonDraw(position, rotate, color, size, size);

  glActiveTexture(GL_TEXTURE0);
  Use(&texture);

  buffer_->Draw();
}

void SpriteRenderer::CommonDraw(const vec2f &position, float rotate,
                                const Rgba &color, const vec2f &size, const vec2f &scale) const {
  Use(shader_);
  const mat4f model = mat4f::Identity()
    .Translate(vec3f(position, 0.0f))
    .Translate(vec3f(-0.5f * size.x, -0.5f * size.y, 0.0f)) // this lets us move the sprite according to the anchor/center point
    .Translate(vec3f(0.5f * size.x, 0.5f * size.y, 0.0f)) // translate sprite to center
    .Rotate(AxisAngle::RightHandAround(vec3f::ZAxis(), Angle::FromRadians(rotate))) // rotate around center
    .Translate(vec3f(-0.5f * size.x, -0.5f * size.y, 0.0f)) // translate back

  .Scale(vec3f(scale, 1.0f));

  shader_->SetUniform(model_, model);
  shader_->SetUniform(color_, color);
}

void SpriteRenderer::DrawNinepatch(const ScalableSprite& ninepatch, const vec2f& position,
                                   float rotate,
                                   const vec2f& scale,
                                   const Rgba& color){
  // const vec2f size = scale;
  const auto half = ninepatch.GetSize()/2.0f;
  const vec2f d(half.GetWidth()*(scale.x-1.0f), half.GetHeight()*(scale.y-1.0f));
  CommonDraw(position-d, rotate, color, scale, scale);

  glActiveTexture(GL_TEXTURE0);
  Use(ninepatch.texture_ptr());

  ninepatch.buffer_ptr()->Draw(); // todo: fix this
  // vao_->Draw();
}

void SpriteRenderer::InitRenderData() {
  BufferBuilder2d data;

  Point a(0.0f, 1.0f, 0.0f, 1.0f);
  Point b(1.0f, 0.0f, 1.0f, 0.0f);
  Point c(0.0f, 0.0f, 0.0f, 0.0f);
  Point d(1.0f, 1.0f, 1.0f, 1.0f);

  data.AddQuad(c, b, a, d);

  buffer_.reset(new Buffer2d(data));
}
