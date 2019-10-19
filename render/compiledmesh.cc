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
        : ambient(core::Color::White)
        , diffuse(core::Color::White)
        , specular(core::Color::White)
        , shininess(135.0f)
    {}

    void
    CompiledMeshMaterial::SetTexture(
            const core::EnumValue&     name,
            std::shared_ptr<Texture2d> texture)
    {
        if(textures.find(name) != textures.end())
        {
            LOG_WARN(name.ToString() << " is already assigned, overwriting...");
        }
        textures[name] = std::move(texture);
    }

    void
    CompiledMeshMaterial::Apply(
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light&       light) const
    {
        shader->UseShader();

        // set common constants
        shader->SetModel(model_matrix);
        shader->SetProjection(projection_matrix);
        shader->SetView(view_matrix);
        shader->SetupLight(light, camera);

        shader->SetColors(ambient, diffuse, specular, shininess);

        // bind all textures
        int texture_index = 0;
        for(const auto& binding: shader->bindings)
        {
            auto       texture = textures.find(binding.name);
            if(texture == textures.end())
            {
                // todo: this is a error and should have been caught by the Validate,
                // abort?
                continue;
            }

            // todo: refactor to material shader
            BindTextureToShader(
                    texture->second.get(),
                    &shader->shader_,
                    binding.uniform,
                    texture_index);
            texture_index += 1;
        }
    }

    void
    CompiledMeshMaterial::LoadDefaultMaterialsFromShader(TextureCache* cache)
    {
        for(const auto& default_texture: shader->default_textures)
        {
            const bool missing = textures.find(default_texture.name)
                                 == textures.end();
            if(missing)
            {
                textures[default_texture.name]
                        = cache->GetTexture(default_texture.path);
            }
        }
    }

    bool
    CompiledMeshMaterial::Validate() const
    {
        std::set<core::EnumValue> shader_values;

        ASSERT(shader);

        bool ok = true;

        for(const auto& binding: shader->bindings)
        {
            shader_values.insert(binding.name);
            const bool missing = textures.find(binding.name) == textures.end();
            if(missing)
            {
                LOG_ERROR(
                        "Material is missing shader-required texture: "
                        << binding.name.ToString());
                ok = false;
            }
        }

        for(const auto& texture: textures)
        {
            const auto name    = texture.first;
            const bool missing = shader_values.find(name) == shader_values.end();
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
            mat.ambient   = material_src.ambient;
            mat.diffuse   = material_src.diffuse;
            mat.specular  = material_src.specular;
            mat.shininess = material_src.shininess;

            std::string shader_name = material_src.shader;
            if(shader_name.empty())
            {
                // todo(Gustav): determine better shader name
                // perhaps by setting a few default shaders on a "project" and we try to
                // match a shader to the object
                shader_name = "default_shader";
            }
            mat.shader = shader_cache->Get(shader_name);
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

            PointLayout::Bind(&part->config);
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
            PointLayout::Bind(nullptr);

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

            PointLayout::Bind(&part->config);
            IndexBuffer::Bind(&part->tris);
            part->tris.Draw(part->tri_count);
            IndexBuffer::Bind(nullptr);
            PointLayout::Bind(nullptr);
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
            PointLayout::Bind(&part->config);
            IndexBuffer::Bind(&part->tris);
            part->tris.Draw(part->tri_count);
            IndexBuffer::Bind(nullptr);
            PointLayout::Bind(nullptr);
        }
    }

}  // namespace euphoria::render
