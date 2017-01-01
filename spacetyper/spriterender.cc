#include "spacetyper/spriterender.h"
#include "spacetyper/vao.h"
#include "spacetyper/scalablesprite.h"

SpriteRenderer::SpriteRenderer(Shader* shader) : shader_(shader) {
  shader_ = shader;
  InitRenderData();
}

SpriteRenderer::~SpriteRenderer() { vao_.reset(); }

void SpriteRenderer::DrawSprite(const Texture2d& texture,
                                const vec2f& position, GLfloat rotate,
                                const vec2f& scale,
                                const vec4f& color) {
  const vec2f size(scale.x * texture.width(), scale.y * texture.height());
  CommonDraw(position, rotate, color, size, size);

  glActiveTexture(GL_TEXTURE0);
  Use(&texture);

  vao_->Draw();
}

void SpriteRenderer::CommonDraw(const vec2f &position, GLfloat rotate,
                                const vec4f &color, const vec2f &size, const vec2f &scale) const {
  Use(shader_);
  const mat4f model = mat4f::Identity()
    .Translate(vec3f(position, 0.0f))
    .Translate(vec3f(-0.5f * size.x, -0.5f * size.y, 0.0f)) // this lets us move the sprite according to the anchor/center point
    .Translate(vec3f(0.5f * size.x, 0.5f * size.y, 0.0f)) // translate sprite to center
    .Rotate(AxisAngle::RightHandAround(vec3f::ZAxis(), Angle::FromRadians(rotate))) // rotate around center
    .Translate(vec3f(-0.5f * size.x, -0.5f * size.y, 0.0f)) // translate back

  .Scale(vec3f(scale, 1.0f));

  shader_->SetMatrix4("model", model);
  shader_->SetVector4f("spriteColor", color);
}

void SpriteRenderer::DrawNinepatch(const ScalableSprite& ninepatch, const vec2f& position,
                   GLfloat rotate,
                   const vec2f& scale,
                   const vec4f& color){
  const vec2f nps = vec2f(1.0f, 1.0f); // ninepatch.GetSize();
  const vec2f size(scale.x * nps.x, scale.y * nps.y);
  CommonDraw(position, rotate, color, size, scale);

  glActiveTexture(GL_TEXTURE0);
  Use(ninepatch.texture_ptr());

  ninepatch.vao_ptr()->Draw();
  // vao_->Draw();
}

void SpriteRenderer::InitRenderData() {
  VaoBuilder data;

  Point a(0.0f, 1.0f, 0.0f, 1.0f);
  Point b(1.0f, 0.0f, 1.0f, 0.0f);
  Point c(0.0f, 0.0f, 0.0f, 0.0f);
  Point d(1.0f, 1.0f, 1.0f, 1.0f);

  data.quad(c, b, a, d);

  vao_.reset(new Vao(data));
}
