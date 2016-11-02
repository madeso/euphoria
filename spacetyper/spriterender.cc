

/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "spacetyper/spriterender.h"


SpriteRenderer::SpriteRenderer(Shader* shader) : shader_(shader)
{
  shader_ = shader;
  InitRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
  glDeleteVertexArrays(1, &vao_);
}

void SpriteRenderer::DrawSprite(const Texture2d &texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
  Use(shader_);
  glm::mat4 model;
  model = glm::translate(model, glm::vec3(position, 0.0f));

  model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
  model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

  model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

  shader_->SetMatrix4("model", model);

  // Render textured quad
  shader_->SetVector3f("spriteColor", color);

  glActiveTexture(GL_TEXTURE0);
  texture.Bind();

  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void SpriteRenderer::InitRenderData()
{
  // Configure VAO/VBO
  GLuint VBO;
  GLfloat vertices[] = {
      // Pos      // Tex
      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f
  };

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(vao_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

