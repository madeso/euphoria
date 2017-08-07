#include "render/shader.h"

#include "core/assert.h"
#include "core/log.h"
#include "core/stringutils.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "render/gl.h"
#include "render/texture.h"

#include "core/filesystem.h"

LOG_SPECIFY_DEFAULT_LOGGER("render.shader")

ShaderId::ShaderId() : id_(glCreateProgram()) {}

ShaderId::~ShaderId() { glDeleteProgram(id_); }

GLuint ShaderId::id() const { return id_; }

namespace {
const Shader *&currentShader() {
  static const Shader *s = nullptr;
  return s;
}
}

bool ShaderId::IsCurrentlyBound() const { return this == currentShader(); }

void Use(const Shader *shader) {
  if (shader != nullptr) {
    glUseProgram(shader->id());
  }
  else {
    glUseProgram(0);
  }
  currentShader() = shader;
}

const Shader* Shader::CurrentlyBound() {
  return currentShader();
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
  LOG_ERROR("ERROR::SHADER: Compile-time error: Type: " << type
            << Trim(log)
            << "\n --------------------------------------------------------- ");
}

bool PrintErrorProgram(GLuint program) {
  if (GetProgramLinkStatus(program)) return true;
  const std::string &log = GetProgramLog(program);
  ReportError(log, "PROGRAM");
  return false;
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
  glBindAttribLocation(id(), attribute.id, attribute.name.c_str());
  bound_attributes_.push_back(attribute);
}

bool Shader::Compile(const GLchar *vertexSource, const GLchar *fragmentSource,
                     const GLchar *geometrySource) {
  bool ret = true;

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
  const bool link_error = PrintErrorProgram(id());
  if( link_error == false ) {
    ret = false;
  }

  glDeleteShader(sVertex);
  glDeleteShader(sFragment);
  if (geometrySource != nullptr) glDeleteShader(gShader);

  for(const auto& attribute : bound_attributes_) {
    int attribute_id = glGetAttribLocation(id(), attribute.name.c_str());
    if( attribute_id == attribute.id ) continue;
    if( attribute_id == -1 ) continue;
    std::cerr << attribute.name << " was bound to " << attribute_id << " but was requested at " << attribute.id << "\n";
    ret = false;
  }

  return ret;
}

ShaderUniform Shader::GetUniform(const std::string& name) {
  int uniform_id = glGetUniformLocation(id(), name.c_str());
  ShaderUniform uniform(name, uniform_id, this);
  bound_uniforms_.push_back(uniform);

  if(uniform.id == -1) {
    std::cerr << "Failed to load " << uniform.name << " from shader " << shader_name_ << "\n";
  }

  return uniform;
}

void Shader::SetUniform(const ShaderUniform& attribute, float val) {
  Assert(IsCurrentlyBound());
  Assert(HasBoundUniform(attribute));
  glUniform1f(attribute.id, val);
}


void Shader::SetUniform(const ShaderUniform& attribute, glint val) {
  Assert(IsCurrentlyBound());
  Assert(HasBoundUniform(attribute));
  glUniform1i(attribute.id, val);
}

void Shader::SetUniform(const ShaderUniform& attribute, const Rgb& val) {
  Assert(IsCurrentlyBound());
  Assert(HasBoundUniform(attribute));
  if(attribute.id == -1) return;
  glUniform3f(attribute.id, val.GetRed(), val.GetGreen(), val.GetBlue());
}

void Shader::SetUniform(const ShaderUniform& attribute, const Rgba& val)  {
  Assert(IsCurrentlyBound());
  Assert(HasBoundUniform(attribute));
  if(attribute.id == -1) return;
  glUniform4f(attribute.id, val.GetRed(), val.GetGreen(), val.GetBlue(), val.GetAlpha());
}

void Shader::SetUniform(const ShaderUniform& attribute, const vec4f& val) {
  Assert(IsCurrentlyBound());
  Assert(HasBoundUniform(attribute));
  if(attribute.id == -1) return;
  glUniform4f(attribute.id, val.x, val.y, val.z, val.w);
}

void Shader::SetUniform(const ShaderUniform& attribute, const mat4f& val) {
  Assert(IsCurrentlyBound());
  Assert(HasBoundUniform(attribute));
  if(attribute.id == -1) return;
  glUniformMatrix4fv(attribute.id, 1, GL_FALSE, val.GetDataPtr());
}

Shader::Shader() {}

namespace {
std::string LoadPath(FileSystem* fs, const std::string& path) {
  // todo: replace with a template instead of basic string
  std::string content;
  if( false == fs->ReadFileToString(path, &content) ) {
    return "";
  }
  else {
    return content;
  }
#if 0
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
#endif
}
}

bool Shader::Load(FileSystem* fs, const std::string& file_path) {
  shader_name_ = file_path;
  auto vert = LoadPath(fs, file_path + ".vert");
  auto frag = LoadPath(fs, file_path + ".frag");
  auto geom = LoadPath(fs, file_path + ".geom");
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

  fail = Compile(vert.c_str(), frag.c_str(), geom.empty() ? nullptr : geom.c_str());
  if( fail == false ) {
    std::cerr << "Failed to compile shader " << file_path << "\n";
  }
  return fail;
}

const std::vector<ShaderAttribute>& Shader::GetAttributes() const {
  return bound_attributes_;
}

const std::string& Shader::GetName() const {
  return shader_name_;
}

bool Shader::HasBoundAttribute(const ShaderAttribute &attribute) const {
  return std::find(bound_attributes_.begin(), bound_attributes_.end(), attribute)
      != bound_attributes_.end();
}

bool Shader::HasBoundUniform(const ShaderUniform& uniform) const {
  return std::find(bound_uniforms_.begin(), bound_uniforms_.end(), uniform)
         != bound_uniforms_.end();
}

void BindTextureToShader(Texture2d* texture, Shader* shader, const ShaderUniform& attribute, glint index)
{
  Assert(index < 16); // at most 16 texture units
  GLenum gl_id = GL_TEXTURE0 + index;
  glActiveTexture(gl_id);
  Use(texture);
  shader->SetUniform(attribute, index);
}
