#include "render/spriterender.h"
#include "render/buffer2d.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/gl.h"
#include "render/bufferbuilder2d.h"

//////////////////////////////////////////////////////////////////////////

DrawData&
DrawData::Rotation(const Angle& r)
{
  rotation = r;
  return *this;
}

DrawData&
DrawData::Scale(const vec2f& s)
{
  scale = s;
  return *this;
}

DrawData&
DrawData::Tint(const Rgba& t)
{
  tint = t;
  return *this;
}

//////////////////////////////////////////////////////////////////////////

SpriteRenderer::SpriteRenderer(Shader* shader)
    : shader_(shader)
    , color_(shader->GetUniform("color"))
    , model_(shader->GetUniform("model"))
    , texture_area_(shader->GetUniform("region"))
{
  shader_ = shader;
  InitRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
  buffer_.reset();
}

void
SpriteRenderer::DrawRect(
    const Texture2d& texture,
    const Rectf&     sprite_area,
    const Rectf&     texture_region,
    const Angle&     rotation_angle,
    const vec2f&     rotation_anchor,
    const Rgba&      tint_color)
{
  Use(shader_);
  vec3f rotation_anchor_displacement{
      -rotation_anchor.x * sprite_area.GetWidth(),
      (rotation_anchor.y - 1) * sprite_area.GetHeight(),
      0.0f};
  const mat4f model =
      mat4f::Identity()
          .Translate(vec3f(sprite_area.BottomLeft(), 0.0f))
          .Translate(-rotation_anchor_displacement)
          .Rotate(AxisAngle::RightHandAround(
              vec3f::ZAxis(),
              rotation_angle))  // rotate around center
          .Translate(rotation_anchor_displacement)
          .Scale(vec3f{sprite_area.GetWidth(), sprite_area.GetHeight(), 1.0f});

  shader_->SetUniform(model_, model);
  shader_->SetUniform(color_, tint_color);
  shader_->SetUniform(texture_area_, texture_region);

  glActiveTexture(GL_TEXTURE0);
  Use(&texture);
  buffer_->Draw();
}

void
SpriteRenderer::DrawSprite(
    const Texture2d& texture, const Rectf& position, const DrawData& data)
{
  DrawRect(
      texture,
      position,
      Rectf::FromTopLeftWidthHeight(1, 0, 1, 1),
      data.rotation,
      vec2f{0.5f, 0.5f},
      data.tint);
}

void
SpriteRenderer::DrawNinepatch(
    const ScalableSprite& ninepatch, const Rectf& rect, const Rgba& tint)
{
  ninepatch.Render(this, rect, tint);
}

void
SpriteRenderer::InitRenderData()
{
  BufferBuilder2d data;

  Point a(0.0f, 1.0f, 0.0f, 1.0f);
  Point b(1.0f, 0.0f, 1.0f, 0.0f);
  Point c(0.0f, 0.0f, 0.0f, 0.0f);
  Point d(1.0f, 1.0f, 1.0f, 1.0f);

  data.AddQuad(c, b, a, d);

  buffer_ = std::make_unique<Buffer2d>(data);
}
