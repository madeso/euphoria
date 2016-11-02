#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "spacetyper/gl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "spacetyper/shader.h"
#include "spacetyper/texture.h"

class SpriteRenderer {
 public:
  SpriteRenderer(Shader* shader);
  ~SpriteRenderer();

  void DrawSprite(const Texture2d& texture, const glm::vec2& position,
                  GLfloat rotate = 0.0f,
                  const glm::vec2& scale = glm::vec2(1, 1),
                  const glm::vec3& color = glm::vec3(1.0f));

 private:
  Shader* shader_;
  GLuint vao_;

  void InitRenderData();
};

#endif