#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "spacetyper/gl.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include "spacetyper/shader.h"
#include "spacetyper/texture.h"

class Vao;
class ScalableSprite;

class SpriteRenderer {
 public:
  SpriteRenderer(Shader* shader);
  ~SpriteRenderer();

  void DrawSprite(const Texture2d& texture, const glm::vec2& position,
                  GLfloat rotate = 0.0f,
                  const glm::vec2& scale = glm::vec2(1, 1),
                  const glm::vec4& color = glm::vec4(1.0f));
  void DrawNinepatch(const ScalableSprite& ninepatch, const glm::vec2& position,
                  GLfloat rotate = 0.0f,
                  const glm::vec2& scale = glm::vec2(1, 1),
                  const glm::vec4& color = glm::vec4(1.0f));

 private:
  Shader* shader_;
  std::unique_ptr<Vao> vao_;

  void InitRenderData();

  void CommonDraw(const glm::vec2 &position, GLfloat rotate, const glm::vec4 &color,
             const glm::vec2 &size, const glm::vec2 &scale) const;
};

#endif