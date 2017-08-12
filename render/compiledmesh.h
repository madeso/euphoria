#ifndef EUPHORIA_COMPILEDMESH_H
#define EUPHORIA_COMPILEDMESH_H

#include <memory>
#include <map>

#include "core/mesh.h"
#include "core/mat4.h"
#include "render/buffer.h"

class Texture2d;
class MaterialShader;
class Light;

class MaterialShaderCache;
class TextureCache;

// one part of the mesh, single material
class CompiledMeshPart
{
 public:
  Vbo          data;
  Vao          config;
  Ebo          tris;
  int          tri_count;
  unsigned int material;
};

class CompiledMeshMaterial
{
 public:
  CompiledMeshMaterial();

  void
  SetShader(std::shared_ptr<MaterialShader> shader);
  void
  SetColors(const Rgb& ambient, const Rgb& diffuse, const Rgb& specular,
            float shininess);

  void
  SetTexture(const EnumValue& name, std::shared_ptr<Texture2d> texture);

  void
  Apply(const mat4f& model_matrix, const mat4f& projection_matrix,
        const mat4f& view_matrix, const vec3f& camera,
        const Light& light) const;

  // asks the shader if all the textures are set, and if more than necessary are
  // set
  bool
  Validate() const;

 private:
  Rgb                             ambient_;
  Rgb                             diffuse_;
  Rgb                             specular_;
  float                           shininess_;
  std::shared_ptr<MaterialShader> shader_;
  std::map<EnumValue, std::shared_ptr<Texture2d>> textures_;
};

// a collection of parts making up a mesh
class CompiledMesh
{
 public:
  std::vector<std::shared_ptr<CompiledMeshPart>> parts;
  std::vector<CompiledMeshMaterial>              materials;

  void
  Render(const mat4f& model_matrix, const mat4f& projection_matrix,
         const mat4f& view_matrix, const vec3f& camera, const Light& light);
};

std::shared_ptr<CompiledMesh>
CompileMesh(const Mesh& mesh, MaterialShaderCache* shader_cache,
            TextureCache* texture_cache);

#endif  // EUPHORIA_COMPILEDMESH_H
