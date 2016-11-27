#include "spacetyper/spriterender.h"
#include "spacetyper/vao.h"
#include "spacetyper/scalablesprite.h"

SpriteRenderer::SpriteRenderer(Shader* shader) : shader_(shader) {
  shader_ = shader;
  InitRenderData();
}

SpriteRenderer::~SpriteRenderer() { vao_.reset(); }

void SpriteRenderer::DrawSprite(const Texture2d& texture,
                                const glm::vec2& position, GLfloat rotate,
                                const glm::vec2& scale,
                                const glm::vec4& color) {
  const glm::vec2 size(scale.x * texture.width(), scale.y * texture.height());
  CommonDraw(position, rotate, color, size, size);

  glActiveTexture(GL_TEXTURE0);
  Use(&texture);

  vao_->Draw();
}

void SpriteRenderer::CommonDraw(const glm::vec2 &position, GLfloat rotate,
                                const glm::vec4 &color, const glm::vec2 &size, const glm::vec2 &scale) const {
  Use(shader_);
  glm::mat4 model;
  model = translate(model, glm::vec3(position, 0.0f));
  model =  // this lets us move the sprite according to the anchor/center point
      translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

  model = translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
  model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
  model =
      translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

  model = glm::scale(model, glm::vec3(scale, 1.0f));

  shader_->SetMatrix4("model", model);
  shader_->SetVector4f("spriteColor", color);
}

void SpriteRenderer::DrawNinepatch(const ScalableSprite& ninepatch, const glm::vec2& position,
                   GLfloat rotate,
                   const glm::vec2& scale,
                   const glm::vec4& color){
  const glm::vec2 size(scale.x * ninepatch.width(), scale.y * ninepatch.height());
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
