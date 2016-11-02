#include "spacetyper/spriterender.h"

SpriteRenderer::SpriteRenderer(Shader* shader) : shader_(shader) {
  shader_ = shader;
  InitRenderData();
}

SpriteRenderer::~SpriteRenderer() { glDeleteVertexArrays(1, &vao_); }

void SpriteRenderer::DrawSprite(const Texture2d& texture,
                                const glm::vec2& position, GLfloat rotate,
                                const glm::vec2& scale,
                                const glm::vec3& color) {
  const glm::vec2 size(scale.x * texture.width(), scale.y * texture.height());
  Use(shader_);
  glm::mat4 model;
  model = glm::translate(model, glm::vec3(position, 0.0f));

  model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
  model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
  model =
      glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

  model = glm::scale(model, glm::vec3(size, 1.0f));

  shader_->SetMatrix4("model", model);
  shader_->SetVector3f("spriteColor", color);

  glActiveTexture(GL_TEXTURE0);
  texture.Bind();

  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void SpriteRenderer::InitRenderData() {
  GLuint VBO;
  GLfloat vertices[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
                        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(vao_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        static_cast<GLvoid*>(nullptr));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
