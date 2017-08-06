#include "render/compiledmesh.h"

#include <set>

#include "core/rgb.h"
#include "core/assert.h"
#include "core/enum.h"
#include "core/texturetypes.h"

#include "render/materialshader.h"
#include "render/shaderattribute3d.h"

#include "materialshadercache.h"
#include "texturecache.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CompiledMeshMaterial::CompiledMeshMaterial()
  : diffuse_( Rgb::From(Color::White) )
{
}

void CompiledMeshMaterial::SetShader(std::shared_ptr<MaterialShader> shader)
{
  Assert(shader);
  shader_ = shader;
}

void CompiledMeshMaterial::SetDiffuse(const Rgb& color)
{
  diffuse_ = color;
}

void CompiledMeshMaterial::SetTexture(const EnumValue& name, std::shared_ptr<Texture2d> texture)
{
  if(textures_.find(name) != textures_.end())
  {
    std::cout << name.ToString() << " is already assigned, overwriting...\n";
  }
  textures_[name] = texture;
}

void CompiledMeshMaterial::Apply(const mat4f& model_matrix, const mat4f& projection_matrix, const mat4f& view_matrix) const
{
  shader_->UseShader();

  // set common constants
  shader_->SetModel(model_matrix);
  shader_->SetProjection(projection_matrix);
  shader_->SetView(view_matrix);

  // bind all textures
  const auto& bindings = shader_->GetBindings();

  int texture_index = 0;
  for(const auto& binding : bindings)
  {
    const auto name = binding.GetName();
    auto texture = textures_.find(name);
    if(texture == textures_.end()) {
      // todo: this is a error and should have been caught by the Validate, abort?
      continue;
    }
    BindTextureToShader(texture->second.get(), &shader_->shader_, binding.GetUniform(), texture_index);
    texture_index += 1;
  }
}

// asks the shader if all the textures are set, and if more than necessary are set
bool CompiledMeshMaterial::Validate() const
{
  std::set<EnumValue> values;

  Assert(shader_);

  const auto bindings = shader_->GetBindings();

  bool ok = true;

  for(const auto& binding : bindings)
  {
    const auto name = binding.GetName();
    values.insert(name);
    const bool missing = textures_.find(name) == textures_.end();
    if(missing) {
      std::cerr << "Material is missing shader required texture " << name.ToString();
      ok = false;
    }
  }

  for(const auto& texture : textures_) {
    const auto name = texture.first;
    const bool missing = values.find(name) == values.end();
    if(missing) {
      std::cerr << "Texture " << name.ToString() << " is specified but is missing in shader\n";
      ok = false;
    }
  }

  return ok;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace // local
{
  DEFINE_ENUM_VALUE(TextureType, DiffuseType, "Diffuse");
}

std::shared_ptr<CompiledMesh> CompileMesh(const Mesh& mesh, MaterialShaderCache* shader_cache, TextureCache* texture_cache) {
  std::shared_ptr<CompiledMesh> ret { new CompiledMesh {} };

  // todo: add default material if there are 0 materials

  int material_index = 0;

  for(const auto& material_src: mesh.materials) {
    material_index += 1;
    CompiledMeshMaterial mat;
    mat.SetDiffuse(material_src.diffuse);

    std::string shader_name = material_src.shader;
    if(shader_name.empty())
    {
      // todo: determine better shader name
      shader_name = "default_shader";
    }
    mat.SetShader( shader_cache->Get(shader_name) );
    for(const auto& texture_src : material_src.textures)
    {
      auto texture = texture_cache->GetTexture(texture_src.path);
      mat.SetTexture(texture_src.type, texture);
    }

    if(!mat.Validate())
    {
      std::cout << "Material " << material_src.name << "(" << material_index << ") failed validation.\n";
    }

    ret->materials.push_back(mat);
  }

  const auto material_count = ret->materials.size();

  for(const auto& part_src: mesh.parts) {
    std::shared_ptr<CompiledMeshPart> part { new CompiledMeshPart{} };

    Vao::Bind(&part->config);
    Vbo::Bind(&part->data);
    Ebo::Bind(&part->tris);

    part->data.SetData(part_src.points);
    part->config.BindVboData(attributes3d::Vertex(), 5 * sizeof(float), 0);
    part->config.BindVboData(attributes3d::TexCoord(), 5 * sizeof(float), 3 * sizeof(float));


    part->tris.SetData(part_src.faces);
    part->tri_count = part_src.facecount;

    Ebo::Bind(nullptr);
    Vbo::Bind(nullptr);
    Vao::Bind(nullptr);

    part->material = part_src.material;

    if( part->material >= material_count ) {
      std::cerr << "Mesh part is using a invalid material, defaulting to first.\n.";
      part->material = 0;
    }

    ret->parts.push_back(part);
  }

  return ret;
}

void CompiledMesh::Render(const mat4f& model_matrix, const mat4f& projection_matrix, const mat4f& view_matrix) {
  for(const auto& part : parts) {
    const CompiledMeshMaterial& material = materials[part->material];

    material.Apply(model_matrix, projection_matrix, view_matrix);

    Vao::Bind(&part->config);
    Ebo::Bind(&part->tris);
    part->tris.Draw(part->tri_count);
    Ebo::Bind(nullptr);
    Vao::Bind(nullptr);
  }
}
