#include "render/compiledmesh.h"

#include <set>
#include <utility>

#include "core/rgb.h"
#include "core/assert.h"
#include "core/enum.h"
#include "core/texturetypes.h"
#include "core/log.h"

#include "render/materialshader.h"
#include "render/shaderattribute3d.h"

#include "materialshadercache.h"
#include "texturecache.h"

LOG_SPECIFY_DEFAULT_LOGGER("core.mesh")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CompiledMeshMaterial::CompiledMeshMaterial()
    : ambient_(Rgb::From(Color::White))
    , diffuse_(Rgb::From(Color::White))
    , specular_(Rgb::From(Color::White))
    , shininess_(135.0f)
{
}

void
CompiledMeshMaterial::SetShader(const std::shared_ptr<MaterialShader>& shader)
{
  ASSERT(shader);
  shader_ = shader;
}

void
CompiledMeshMaterial::SetColors(const Rgb& ambient, const Rgb& diffuse,
                                const Rgb& specular, float shininess)
{
  ambient_   = ambient;
  diffuse_   = diffuse;
  specular_  = specular;
  shininess_ = shininess;
}

void
CompiledMeshMaterial::SetTexture(const EnumValue&           name,
                                 std::shared_ptr<Texture2d> texture)
{
  if(textures_.find(name) != textures_.end())
  {
    LOG_WARN(name.ToString() << " is already assigned, overwriting...");
  }
  textures_[name] = std::move(texture);
}

void
CompiledMeshMaterial::Apply(const mat4f& model_matrix,
                            const mat4f& projection_matrix,
                            const mat4f& view_matrix, const vec3f& camera,
                            const Light& light) const
{
  shader_->UseShader();

  // set common constants
  shader_->SetModel(model_matrix);
  shader_->SetProjection(projection_matrix);
  shader_->SetView(view_matrix);
  shader_->SetupLight(light, camera);

  shader_->SetColors(ambient_, diffuse_, specular_, shininess_);

  // bind all textures
  const auto& bindings = shader_->GetBindings();

  int texture_index = 0;
  for(const auto& binding : bindings)
  {
    const auto name    = binding.GetName();
    auto       texture = textures_.find(name);
    if(texture == textures_.end())
    {
      // todo: this is a error and should have been caught by the Validate,
      // abort?
      continue;
    }

    // todo: refactor to material shader
    BindTextureToShader(texture->second.get(), &shader_->shader_,
                        binding.GetUniform(), texture_index);
    texture_index += 1;
  }
}

// asks the shader if all the textures are set, and if more than necessary are
// set
bool
CompiledMeshMaterial::Validate() const
{
  std::set<EnumValue> values;

  ASSERT(shader_);

  const auto bindings = shader_->GetBindings();

  bool ok = true;

  for(const auto& binding : bindings)
  {
    const auto name = binding.GetName();
    values.insert(name);
    const bool missing = textures_.find(name) == textures_.end();
    if(missing)
    {
      LOG_ERROR("Material is missing shader required texture "
                << name.ToString());
      ok = false;
    }
  }

  for(const auto& texture : textures_)
  {
    const auto name    = texture.first;
    const bool missing = values.find(name) == values.end();
    if(missing)
    {
      LOG_ERROR("Texture " << name.ToString()
                           << " is specified but is missing in shader");
      ok = false;
    }
  }

  return ok;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace  // local
{
  DEFINE_ENUM_VALUE(TextureType, DiffuseType, "Diffuse");  // NOLINT
}  // namespace

class AttributeBinder
{
 public:
  void
  Register(const ShaderAttribute& attribute, int size)
  {
    bind_datas_.emplace_back(BindData{attribute, size});
    total_size_ += size;
  }

  void
  Bind(const std::shared_ptr<CompiledMeshPart>& part)
  {
    int stride = 0;
    for(const auto& d : bind_datas_)
    {
      part->config.BindVboData(d.attribute, total_size_, stride);
      stride += d.size;
    }
  }

 private:
  struct BindData
  {
    BindData(ShaderAttribute a, int s)
        : attribute(std::move(a))
        , size(s)
    {
    }
    ShaderAttribute attribute;
    int             size;
  };

  int                   total_size_ = 0;
  std::vector<BindData> bind_datas_;
};

std::shared_ptr<CompiledMesh>
CompileMesh(const Mesh& mesh, MaterialShaderCache* shader_cache,
            TextureCache* texture_cache)
{
  std::shared_ptr<CompiledMesh> ret{new CompiledMesh{}};

  // todo: add default material if there are 0 materials

  int material_index = 0;

  for(const auto& material_src : mesh.materials)
  {
    material_index += 1;
    CompiledMeshMaterial mat;
    mat.SetColors(material_src.ambient, material_src.diffuse,
                  material_src.specular, material_src.shininess);

    std::string shader_name = material_src.shader;
    if(shader_name.empty())
    {
      // todo: determine better shader name
      // perhaps by setting a few default shaders on a "project" and we try to
      // match a shader to the object
      shader_name = "default_shader";
    }
    mat.SetShader(shader_cache->Get(shader_name));
    for(const auto& texture_src : material_src.textures)
    {
      auto texture = texture_cache->GetTexture(texture_src.path);
      mat.SetTexture(texture_src.type, texture);
    }

    if(!mat.Validate())
    {
      LOG_WARN("Material " << material_src.name << "(" << material_index
                           << ") failed validation.");
    }

    ret->materials.push_back(mat);
  }

  const auto material_count = ret->materials.size();

  for(const auto& part_src : mesh.parts)
  {
    std::shared_ptr<CompiledMeshPart> part{new CompiledMeshPart{}};

    Vao::Bind(&part->config);
    Vbo::Bind(&part->data);
    Ebo::Bind(&part->tris);

    part->data.SetData(part_src.points);
    AttributeBinder binder;
    binder.Register(attributes3d::Vertex(), 3 * sizeof(float));
    binder.Register(attributes3d::Normal(), 3 * sizeof(float));
    binder.Register(attributes3d::TexCoord(), 2 * sizeof(float));
    binder.Bind(part);


    part->tris.SetData(part_src.faces);
    part->tri_count = part_src.facecount;

    Ebo::Bind(nullptr);
    Vbo::Bind(nullptr);
    Vao::Bind(nullptr);

    part->material = part_src.material;

    if(part->material >= material_count)
    {
      LOG_ERROR("Mesh part is using a invalid material, defaulting to first.");
      part->material = 0;
    }

    ret->parts.push_back(part);
  }

  return ret;
}

void
CompiledMesh::Render(const mat4f& model_matrix, const mat4f& projection_matrix,
                     const mat4f& view_matrix, const vec3f& camera,
                     const Light& light)
{
  for(const auto& part : parts)
  {
    const CompiledMeshMaterial& material = materials[part->material];

    material.Apply(model_matrix, projection_matrix, view_matrix, camera, light);

    Vao::Bind(&part->config);
    Ebo::Bind(&part->tris);
    part->tris.Draw(part->tri_count);
    Ebo::Bind(nullptr);
    Vao::Bind(nullptr);
  }
}
