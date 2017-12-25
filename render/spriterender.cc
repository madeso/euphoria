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

SpriteRenderer::~SpriteRenderer()
{
  buffer_.reset();
}

void
SpriteRenderer::DrawSprite(
    const Texture2d& texture,
    const vec2f&     position,
    const Angle&     angle,
    const vec2f&     scale,
    const Rgba&      color)
{
  const vec2f size(scale.x * texture.GetWidth(), scale.y * texture.GetHeight());
  CommonDraw(position, angle, color, size, size);

  glActiveTexture(GL_TEXTURE0);
  Use(&texture);

  buffer_->Draw();
}

void
SpriteRenderer::CommonDraw(
    const vec2f& position,
    const Angle&     angle,
    const Rgba&  color,
    const vec2f& size,
    const vec2f& scale) const
{
  Use(shader_);
  const mat4f model =
      mat4f::Identity()
          .Translate(vec3f(position, 0.0f))
          .Translate(vec3f(
              -0.5f * size.x,
              -0.5f * size.y,
              0.0f))  // this lets us move the sprite according to
                      // the anchor/center point
          .Translate(vec3f(
              0.5f * size.x,
              0.5f * size.y,
              0.0f))  // translate sprite to center
          .Rotate(AxisAngle::RightHandAround(
              vec3f::ZAxis(),
              angle))  // rotate around center
          .Translate(
              vec3f(-0.5f * size.x, -0.5f * size.y, 0.0f))  // translate back

          .Scale(vec3f(scale, 1.0f));

  shader_->SetUniform(model_, model);
  shader_->SetUniform(color_, color);
}

void
SpriteRenderer::DrawNinepatch(
    const ScalableSprite& ninepatch,
    const vec2f&          position,
    const vec2f&          scale,
    const Rgba&           color)
{
  // const vec2f size = scale;
  const auto size = ninepatch.GetSize();
  const auto  half = size / 2.0f;
  const vec2f d(
      half.GetWidth() * (scale.x - 1.0f), half.GetHeight() * (scale.y - 1.0f));
  // todo: always moving up by height, change this in the buffer instead
  CommonDraw(position - d-vec2f{0, size.GetHeight()}, Angle::Zero(), color, scale, scale);

  glActiveTexture(GL_TEXTURE0);
  Use(ninepatch.GetTextureId());

  ninepatch.GetBufferPtr()->Draw();  // todo: fix this
  // vao_->Draw();
}

void
SpriteRenderer::InitRenderData()
{
  BufferBuilder2d data;

  Point a(0.0f, 1.0f, 0.0f, 0.0f);
  Point b(1.0f, 0.0f, 1.0f, 1.0f);
  Point c(0.0f, 0.0f, 0.0f, 1.0f);
  Point d(1.0f, 1.0f, 1.0f, 0.0f);

  data.AddQuad(c, b, a, d);

  buffer_ = std::make_unique<Buffer2d>(data);
}
