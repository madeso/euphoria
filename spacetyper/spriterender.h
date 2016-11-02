#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "spacetyper/gl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "spacetyper/texture.h"
#include "spacetyper/shader.h"


class SpriteRenderer
{
public:
    SpriteRenderer(Shader* shader);
    ~SpriteRenderer();

    void DrawSprite(const Texture2d& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

private:
    Shader* shader_;
    GLuint vao_;

    void InitRenderData();
};

#endif