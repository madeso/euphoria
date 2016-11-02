

/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "spacetyper/shader.h"

#include <cassert>
#include <iostream>
#include <vector>

#include "spacetyper/gl.h"

ShaderId::ShaderId() : id_(glCreateProgram()) {}

ShaderId::~ShaderId() { glDeleteProgram(id_); }

GLuint ShaderId::id() const { return id_; }

namespace {
ShaderId *&currentShader() {
  static ShaderId *s = nullptr;
  return s;
}
}

bool ShaderId::IsCurrentlyBound() const { return this == currentShader(); }

void Use(ShaderId *shader) {
  if (shader != nullptr) {
    glUseProgram(shader->id());
  }
  currentShader() = shader;
}

////////////////////////////////////////////////////////////////////////////////

namespace {
bool GetShaderCompileStatus(GLuint object) {
  int r = GL_TRUE;
  glGetShaderiv(object, GL_COMPILE_STATUS, &r);
  return r == GL_TRUE;
}

bool GetProgramLinkStatus(GLuint object) {
  int r = GL_TRUE;
  glGetProgramiv(object, GL_LINK_STATUS, &r);
  return r == GL_TRUE;
}

std::string GetShaderLog(GLuint shader) {
  int length = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
  if (length <= 0) return "";
  const int max_length = length + 1;
  std::vector<char> str(max_length, 0);
  glGetShaderInfoLog(shader, max_length, &length, &str[0]);
  return &str[0];
}

std::string GetProgramLog(GLuint shader) {
  int length = 0;
  glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
  if (length <= 0) return "";
  const int max_length = length + 1;
  std::vector<char> str(max_length, 0);
  glGetProgramInfoLog(shader, max_length, &length, &str[0]);
  return &str[0];
}
}

void ReportError(const std::string &log, const std::string &type) {
  std::cerr << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
            << log
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
}

void PrintErrorProgram(GLuint program) {
  if (GetProgramLinkStatus(program)) return;
  const std::string &log = GetProgramLog(program);
  ReportError(log, "PROGRAM");
}

void PrintErrorShader(GLuint shader, const std::string &type) {
  if (GetShaderCompileStatus(shader)) return;
  const std::string &log = GetShaderLog(shader);
  ReportError(log, type);
}

GLuint CompileShader(GLuint type, const GLchar *source,
                     const std::string &name) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  PrintErrorShader(shader, name);
  return shader;
}

void Shader::Compile(const GLchar *vertexSource, const GLchar *fragmentSource,
                     const GLchar *geometrySource) {
  GLuint sVertex = CompileShader(GL_VERTEX_SHADER, vertexSource, "VERTEX");
  GLuint sFragment =
      CompileShader(GL_FRAGMENT_SHADER, fragmentSource, "FRAGMENT");

  GLuint gShader = 0;
  if (geometrySource != nullptr) {
    gShader = CompileShader(GL_GEOMETRY_SHADER, geometrySource, "GEOMETRY");
  }

  glAttachShader(id(), sVertex);
  glAttachShader(id(), sFragment);
  if (geometrySource != nullptr) glAttachShader(id(), gShader);
  glLinkProgram(id());
  PrintErrorProgram(id());

  glDeleteShader(sVertex);
  glDeleteShader(sFragment);
  if (geometrySource != nullptr) glDeleteShader(gShader);
}

void Shader::SetFloat(const GLchar *name, GLfloat value) {
  assert(IsCurrentlyBound());
  glUniform1f(glGetUniformLocation(id(), name), value);
}

void Shader::SetInteger(const GLchar *name, GLint value) {
  assert(IsCurrentlyBound());
  glUniform1i(glGetUniformLocation(id(), name), value);
}

void Shader::SetVector2f(const GLchar *name, GLfloat x, GLfloat y) {
  assert(IsCurrentlyBound());
  glUniform2f(glGetUniformLocation(id(), name), x, y);
}

void Shader::SetVector2f(const GLchar *name, const glm::vec2 &value) {
  assert(IsCurrentlyBound());
  glUniform2f(glGetUniformLocation(id(), name), value.x, value.y);
}

void Shader::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z) {
  assert(IsCurrentlyBound());
  glUniform3f(glGetUniformLocation(id(), name), x, y, z);
}

void Shader::SetVector3f(const GLchar *name, const glm::vec3 &value) {
  assert(IsCurrentlyBound());
  glUniform3f(glGetUniformLocation(id(), name), value.x, value.y, value.z);
}

void Shader::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z,
                         GLfloat w) {
  assert(IsCurrentlyBound());
  glUniform4f(glGetUniformLocation(id(), name), x, y, z, w);
}

void Shader::SetVector4f(const GLchar *name, const glm::vec4 &value) {
  assert(IsCurrentlyBound());
  glUniform4f(glGetUniformLocation(id(), name), value.x, value.y, value.z,
              value.w);
}

void Shader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix) {
  assert(IsCurrentlyBound());
  glUniformMatrix4fv(glGetUniformLocation(id(), name), 1, GL_FALSE,
                     glm::value_ptr(matrix));
}

Shader::Shader() {}

Shader::Shader(const GLchar *vertexSource, const GLchar *fragmentSource,
               const GLchar *geometrySource) {
  Compile(vertexSource, fragmentSource, geometrySource);
}
