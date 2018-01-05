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
    const Texture2d& texture, const vec2f& position, const DrawData& data)
{
  DrawData new_data = data;
  new_data.scale    = vec2f{data.scale.x * texture.GetWidth(),
                         data.scale.y * texture.GetHeight()};
  CommonDraw(position, new_data);

  glActiveTexture(GL_TEXTURE0);
  Use(&texture);

  buffer_->Draw();
}

void
SpriteRenderer::CommonDraw(const vec2f& position, const DrawData& data) const
{
  Use(shader_);
  const mat4f model = mat4f::Identity()
                          .Translate(vec3f(position, 0.0f))
                          .Rotate(AxisAngle::RightHandAround(
                              vec3f::ZAxis(),
                              data.rotation))  // rotate around center
                          .Scale(vec3f(data.scale, 1.0f))
                          .Translate(vec3f(-0.5f, -0.5f, 0.0f));

  shader_->SetUniform(model_, model);
  shader_->SetUniform(color_, data.tint);
}

void
SpriteRenderer::DrawNinepatch(
    const ScalableSprite& ninepatch,
    const vec2f&          position,
    const DrawData&       data)
{
  // const vec2f size = scale;
  const auto  size = ninepatch.GetSize();
  const auto  half = size / 2.0f;
  const vec2f d(
      half.GetWidth() * (data.scale.x - 1.0f),
      half.GetHeight() * (data.scale.y - 1.0f));
  // todo: always moving up by height, change this in the buffer instead
  CommonDraw(position - d - vec2f{0, size.GetHeight()}, data);

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
