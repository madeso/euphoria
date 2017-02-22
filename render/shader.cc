#include "render/shader.h"

#include "core/assert.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include "render/gl.h"

ShaderId::ShaderId() : id_(glCreateProgram()) {}

ShaderId::~ShaderId() { glDeleteProgram(id_); }

GLuint ShaderId::id() const { return id_; }

namespace {
const ShaderId *&currentShader() {
  static const ShaderId *s = nullptr;
  return s;
}
}

bool ShaderId::IsCurrentlyBound() const { return this == currentShader(); }

void Use(const ShaderId *shader) {
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

void Shader::PreBind(const ShaderAttribute& attribute) {
  Assert(this);
  Assert(IsCurrentlyBound());
  glBindAttribLocation(id(), attribute.id, attribute.name.c_str());
  bound_attributes_.push_back(attribute);
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

void Shader::SetUniform(const ShaderAttribute& attribute, glint val) {
  Assert(this);
  Assert(IsCurrentlyBound());
  Assert(HasBoundAttribute(attribute));
  Assert(attribute.size == ShaderAttributeSize::VEC1);
  glUniform1i(attribute.id, val);
}

void Shader::SetUniform(const ShaderAttribute& attribute, const Rgb& val) {
  Assert(this);
  Assert(IsCurrentlyBound());
  Assert(HasBoundAttribute(attribute));
  Assert(attribute.size == ShaderAttributeSize::VEC3);
  glUniform3f(attribute.id, val.GetRed(), val.GetGreen(), val.GetBlue());
}

void Shader::SetUniform(const ShaderAttribute& attribute, const Rgba& val)  {
  Assert(this);
  Assert(IsCurrentlyBound());
  Assert(HasBoundAttribute(attribute));
  Assert(attribute.size == ShaderAttributeSize::VEC4);
  glUniform4f(attribute.id, val.GetRed(), val.GetGreen(), val.GetBlue(), val.GetAlpha());
}

void Shader::SetUniform(const ShaderAttribute& attribute, const vec4f& val) {
  Assert(this);
  Assert(IsCurrentlyBound());
  Assert(HasBoundAttribute(attribute));
  Assert(attribute.size == ShaderAttributeSize::VEC4);
  glUniform4f(attribute.id, val.x, val.y, val.z, val.w);
}

void Shader::SetUniform(const ShaderAttribute& attribute, const mat4f& val) {
  Assert(this);
  Assert(IsCurrentlyBound());
  Assert(HasBoundAttribute(attribute));
  Assert(attribute.size == ShaderAttributeSize::MAT44);
  glUniformMatrix4fv(attribute.id, 1, GL_FALSE, val.GetDataPtr());
}

Shader::Shader() {}

namespace {
std::string LoadPath(const std::string& path) {
  std::ifstream t(path.c_str());

  if( !t ) {
    return "";
  }

  std::string str;

  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)),
             std::istreambuf_iterator<char>());
  return str;
}
}

bool Shader::Load(const std::string& file_path) {
  auto vert = LoadPath(file_path + ".vert");
  auto frag = LoadPath(file_path + ".frag");
  auto geom = LoadPath(file_path + ".geom");
  bool fail = false;
  if( vert.empty() ) {
    std::cerr << "Failed to load vert shader " << file_path << "\n";
    fail = true;
  }
  if( frag.empty() ) {
    std::cerr << "Failed to load frag shader " << file_path << "\n";
    fail = true;
  }
  if( fail ) {
    return false;
  }

  Compile(vert.c_str(), frag.c_str(), geom.empty() ? nullptr : geom.c_str());
  return true;
}

bool Shader::HasBoundAttribute(const ShaderAttribute &attribute) const {
  return std::find(bound_attributes_.begin(), bound_attributes_.end(), attribute)
      != bound_attributes_.end();
}
