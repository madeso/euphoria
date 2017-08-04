#ifndef EUPHORIA_MATERIALSHADER_H
#define EUPHORIA_MATERIALSHADER_H

#include "render/shader.h"

class MaterialShader {
 public:
  MaterialShader();

  bool Load(FileSystem* file_system, const std::string& path);

  void UseShader();

  void SetProjection(const mat4f& projection);
  void SetView(const mat4f& view);
  void SetModel(const mat4f& model);

  // todo: move to private when we support material textures and stuff
  Shader shader_;
 private:
  ShaderUniform projection_;
  ShaderUniform view_;
  ShaderUniform model_;
};


#endif //EUPHORIA_MATERIALSHADER_H
