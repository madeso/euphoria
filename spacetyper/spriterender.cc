#include "spacetyper/spriterender.h"
#include "spacetyper/vao.h"

SpriteRenderer::SpriteRenderer(Shader* shader) : shader_(shader) {
  shader_ = shader;
  InitRenderData();
}

SpriteRenderer::~SpriteRenderer() { vao_.reset(); }

void SpriteRenderer::DrawSprite(const Texture2d& texture,
                                const glm::vec2& position, GLfloat rotate,
                                const glm::vec2& scale,
                                const glm::vec3& color) {
  const glm::vec2 size(scale.x * texture.width(), scale.y * texture.height());
  Use(shader_);
  glm::mat4 model;
  model = glm::translate(model, glm::vec3(position, 0.0f));
  model =  // this lets us move the sprite according to the anchor/center point
      glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

  model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
  model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
  model =
      glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

  model = glm::scale(model, glm::vec3(size, 1.0f));

  shader_->SetMatrix4("model", model);
  shader_->SetVector3f("spriteColor", color);

  glActiveTexture(GL_TEXTURE0);
  Use(&texture);

  vao_->Draw();
}

void SpriteRenderer::InitRenderData() {
  VaoBuilder data;

  data.quad(Point(0.0f, 1.0f, 0.0f, 1.0f), Point(1.0f, 0.0f, 1.0f, 0.0f),
            Point(0.0f, 0.0f, 0.0f, 0.0f), Point(1.0f, 1.0f, 1.0f, 1.0f));

  vao_.reset(new Vao(data));
}
