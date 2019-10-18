#include "render/compiledmesh.h"

#include <set>
#include <utility>

#include "core/rgb.h"
#include "core/assert.h"
#include "core/enum.h"
#include "core/texturetypes.h"
#include "core/log.h"
#include "core/vfs_path.h"

#include "render/materialshader.h"
#include "render/shaderattribute3d.h"
#include "render/attributebinder.h"
#include "render/materialshadercache.h"
#include "render/texturecache.h"

namespace euphoria::render
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.mesh")

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CompiledMeshMaterial::CompiledMeshMaterial()
        : ambient_(core::Color::White)
        , diffuse_(core::Color::White)
        , specular_(core::Color::White)
        , shininess_(135.0f)
    {}

    void
    CompiledMeshMaterial::SetShader(
            const std::shared_ptr<MaterialShader>& shader)
    {
        ASSERT(shader);
        shader_ = shader;
    }

    void
    CompiledMeshMaterial::SetColors(
            const core::Rgb& ambient,
            const core::Rgb& diffuse,
            const core::Rgb& specular,
            float            shininess)
    {
        ambient_   = ambient;
        diffuse_   = diffuse;
        specular_  = specular;
        shininess_ = shininess;
    }

    void
    CompiledMeshMaterial::SetTexture(
            const core::EnumValue&     name,
            std::shared_ptr<Texture2d> texture)
    {
        if(textures_.find(name) != textures_.end())
        {
            LOG_WARN(name.ToString() << " is already assigned, overwriting...");
        }
        textures_[name] = std::move(texture);
    }

    void
    CompiledMeshMaterial::Apply(
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light&       light) const
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
        for(const auto& binding: bindings)
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
            BindTextureToShader(
                    texture->second.get(),
                    &shader_->shader_,
                    binding.GetUniform(),
                    texture_index);
            texture_index += 1;
        }
    }

    void
    CompiledMeshMaterial::LoadDefaultMaterialsFromShader(TextureCache* cache)
    {
        const auto textures = shader_->GetDefaultTextures();

        for(const auto& texture: textures)
        {
            const bool missing
                    = textures_.find(texture.GetName()) == textures_.end();
            if(missing)
            {
                textures_[texture.GetName()]
                        = cache->GetTexture(texture.GetPath());
            }
        }
    }

    bool
    CompiledMeshMaterial::Validate() const
    {
        std::set<core::EnumValue> values;

        ASSERT(shader_);

        const auto bindings = shader_->GetBindings();

        bool ok = true;

        for(const auto& binding: bindings)
        {
            const auto name = binding.GetName();
            values.insert(name);
            const bool missing = textures_.find(name) == textures_.end();
            if(missing)
            {
                LOG_ERROR(
                        "Material is missing shader required texture "
                        << name.ToString());
                ok = false;
            }
        }

        for(const auto& texture: textures_)
        {
            const auto name    = texture.first;
            const bool missing = values.find(name) == values.end();
            if(missing)
            {
                LOG_ERROR(
                        "Texture " << name.ToString()
                                   << " is specified but is missing in shader");
                ok = false;
            }
        }

        return ok;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace  // local
    {
        DEFINE_ENUM_VALUE(core::TextureType, DiffuseType, "Diffuse");  // NOLINT
    }  // namespace

    std::shared_ptr<CompiledMesh>
    CompileMesh(
            const core::Mesh&      mesh,
            MaterialShaderCache*   shader_cache,
            TextureCache*          texture_cache,
            const core::vfs::Path& texture_folder)
    {
        std::shared_ptr<CompiledMesh> ret {new CompiledMesh {}};

        // todo: add default material if there are 0 materials

        int material_index = 0;

        for(const auto& material_src: mesh.materials)
        {
            material_index += 1;
            CompiledMeshMaterial mat;
            mat.SetColors(
                    material_src.ambient,
                    material_src.diffuse,
                    material_src.specular,
                    material_src.shininess);

            std::string shader_name = material_src.shader;
            if(shader_name.empty())
            {
                // todo: determine better shader name
                // perhaps by setting a few default shaders on a "project" and we try to
                // match a shader to the object
                shader_name = "default_shader";
            }
            mat.SetShader(shader_cache->Get(shader_name));
            for(const auto& texture_src: material_src.textures)
            {
                const auto texture_path
                        = texture_folder.GetFile(texture_src.path)
                                  .GetAbsolutePath();
                auto texture = texture_cache->GetTexture(texture_path);
                mat.SetTexture(texture_src.type, texture);
            }

            mat.LoadDefaultMaterialsFromShader(texture_cache);

            if(!mat.Validate())
            {
                LOG_WARN(
                        "Material " << material_src.name << "("
                                    << material_index
                                    << ") failed validation.");
            }

            ret->materials.push_back(mat);
        }

        const auto material_count = ret->materials.size();

        for(const auto& part_src: mesh.parts)
        {
            std::shared_ptr<CompiledMeshPart> part {new CompiledMeshPart()};

            Vao::Bind(&part->config);
            VertexBuffer::Bind(&part->data);
            IndexBuffer::Bind(&part->tris);

            part->data.SetData(part_src.points);
            AttributeBinder binder;
            // todo: move this to a data file, load the mesh dynamically
            binder.Register(attributes3d::Vertex());
            binder.Register(attributes3d::Normal());
            binder.Register(attributes3d::TexCoord());
            binder.Bind(part);


            part->tris.SetData(part_src.faces);
            part->tri_count = part_src.facecount;

            IndexBuffer::Bind(nullptr);
            VertexBuffer::Bind(nullptr);
            Vao::Bind(nullptr);

            part->material = part_src.material;

            if(part->material >= material_count)
            {
                LOG_ERROR(
                        "Mesh part is using a invalid material, defaulting to first.");
                part->material = 0;
            }

            ret->parts.push_back(part);
        }

        return ret;
    }

    std::vector<std::shared_ptr<CompiledMeshMaterial>>
    CompiledMesh::GetNoOverriddenMaterials() const
    {
        std::vector<std::shared_ptr<CompiledMeshMaterial>> ret;
        ret.reserve(materials.size());

        for(unsigned int i = 0; i < materials.size(); ++i)
        {
            ret.push_back(nullptr);
        }

        return ret;
    }

    void
    CompiledMesh::Render(
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light&       light,
            const std::vector<std::shared_ptr<CompiledMeshMaterial>>&
                    overridden_materials)
    {
        ASSERT(materials.size() == overridden_materials.size());

        for(const auto& part: parts)
        {
            const auto& base_material = materials[part->material];
            const auto& overridden_material
                    = overridden_materials[part->material];
            const auto& material = overridden_material == nullptr
                                           ? base_material
                                           : *overridden_material;

            material.Apply(
                    model_matrix,
                    projection_matrix,
                    view_matrix,
                    camera,
                    light);

            Vao::Bind(&part->config);
            IndexBuffer::Bind(&part->tris);
            part->tris.Draw(part->tri_count);
            IndexBuffer::Bind(nullptr);
            Vao::Bind(nullptr);
        }
    }

    void
    CompiledMesh::BasicRender(
            const core::mat4f&              model_matrix,
            const core::mat4f&              projection_matrix,
            const core::mat4f&              view_matrix,
            std::shared_ptr<MaterialShader> shader)
    {
        shader->SetProjection(projection_matrix);
        shader->SetModel(model_matrix);
        shader->SetView(view_matrix);

        for(const auto& part: parts)
        {
            Vao::Bind(&part->config);
            IndexBuffer::Bind(&part->tris);
            part->tris.Draw(part->tri_count);
            IndexBuffer::Bind(nullptr);
            Vao::Bind(nullptr);
        }
    }

}  // namespace euphoria::render
