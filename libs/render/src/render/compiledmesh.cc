#include "render/compiledmesh.h"

#include <set>
#include <utility>
#include <numeric>

#include "core/cint.h"
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
#include "render/texture.h"
#include "render/actor.h"

namespace euphoria::render
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.mesh")

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CompiledMeshMaterial::CompiledMeshMaterial()
        : ambient(core::Color::White)
        , diffuse(core::Color::White)
        , specular(core::Color::White)
        , shininess(135.0f)
    {
    }


    void
    CompiledMeshMaterial::SetTexture
    (
        const core::EnumValue& name,
        std::shared_ptr<texture2d> texture
    )
    {
        if(textures.find(name) != textures.end())
        {
            LOG_WARN("{0} is already assigned, overwriting...", name.ToString());
        }
        textures[name] = std::move(texture);
    }


    void
    CompiledMeshMaterial::Apply
    (
        const core::mat4f& model_matrix,
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f& camera,
        const Light& light
    ) const
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
            auto texture = textures.find(binding.name);
            if(texture == textures.end())
            {
                // todo(Gustav): this is a error and should have been caught by the Validate,
                // abort?
                continue;
            }

            // todo(Gustav): refactor to material shader
            BindTextureToShader(
                    texture->second.get(),
                    &shader->shader,
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
            const bool missing
                    = textures.find(default_texture.name) == textures.end();
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
                LOG_ERROR
                (
                    "Material is missing shader-required texture: {0}",
                    binding.name.ToString()
                );
                ok = false;
            }
        }

        for(const auto& texture: textures)
        {
            const auto name = texture.first;
            const bool missing
                    = shader_values.find(name) == shader_values.end();
            if(missing)
            {
                LOG_ERROR
                (
                    "Texture {0} is specified but is missing in shader",
                    name.ToString()
                );
                ok = false;
            }
        }

        return ok;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace // local
    {
        DEFINE_ENUM_VALUE(core::TextureType, DiffuseType, "Diffuse"); // NOLINT
    } // namespace


    void
    ConvertPointsToVertexBuffer
    (
        const std::vector<core::MeshPoint>& points,
        const std::vector<ShaderAttribute>& attributes,
        VertexBuffer* vb
    )
    {
        constexpr auto add_float2 = [](std::vector<float>* dst, const core::vec2f& src)
        {
            dst->emplace_back(src.x);
            dst->emplace_back(src.y);
        };
        constexpr auto add_float3 = [](std::vector<float>* dst, const core::vec3f& src)
        {
            dst->emplace_back(src.x);
            dst->emplace_back(src.y);
            dst->emplace_back(src.z);
        };
        std::vector<float> data;
        const auto total_attributes = std::accumulate
        (
            attributes.begin(), attributes.end(), 0,
            [](int count, const ShaderAttribute& att) -> int
            {
                return count + att.GetElementCount();
            }
        );
        data.reserve(total_attributes * points.size());
        for(const auto& point: points)
        {
            for(const auto& att: attributes)
            {
                switch(att.source)
                {
                case ShaderAttributeSource::Vertex:
                    ASSERT(att.type == ShaderAttributeType::FLOAT3);
                    add_float3(&data, point.vertex);
                    break;
                case ShaderAttributeSource::Normal:
                    ASSERT(att.type == ShaderAttributeType::FLOAT3);
                    add_float3(&data, point.normal);
                    break;
                case ShaderAttributeSource::Uv:
                    ASSERT(att.type == ShaderAttributeType::FLOAT2);
                    add_float2(&data, point.uv);
                    break;
                default: DIE("Unhandled case");
                }
            }
        }
        vb->SetData(data);
    }


    void
    ConvertTrisToIndexBuffer(const std::vector<core::MeshFace>& faces, IndexBuffer* b)
    {
        std::vector<unsigned int> data;
        data.reserve(faces.size() * 3);
        for(const auto& f: faces)
        {
            data.emplace_back(core::Cint_to_unsigned_int(f.a));
            data.emplace_back(core::Cint_to_unsigned_int(f.b));
            data.emplace_back(core::Cint_to_unsigned_int(f.c));
        }
        b->SetData(data);
    }

    std::shared_ptr<CompiledMesh>
    CompileMesh
    (
        const core::Mesh& mesh,
        MaterialShaderCache* shader_cache,
        TextureCache* texture_cache,
        const core::vfs::DirPath& texture_folder,
        const std::string& debug_name
    )
    {
        std::shared_ptr<CompiledMesh> ret {new CompiledMesh {}};

        // todo(Gustav): add default material if there are 0 materials

        int material_index = 0;

        for(const auto& material_src: mesh.materials)
        {
            material_index += 1;
            CompiledMeshMaterial mat;
            mat.ambient = material_src.ambient;
            mat.diffuse = material_src.diffuse;
            mat.specular = material_src.specular;
            mat.shininess = material_src.shininess;

            // todo(Gustav): determine a better default shader name
            // perhaps by setting a few default shaders on a "project" and
            // we try to match a shader to the object
            const auto shader_name = material_src.shader.value_or
            (
                core::vfs::FilePath("~/default_shader")
            );
            mat.shader = shader_cache->Get(shader_name);
            for(const auto& texture_src: material_src.textures)
            {
                const auto texture_path = core::vfs::ResolveRelative
                (
                    core::vfs::FilePath{texture_src.path},
                    texture_folder
                );
                if(texture_path.has_value() == false)
                {
                    LOG_WARN
                    (
                        "Invalid texture path {0} with root {1}",
                        texture_src.path,
                        texture_folder.path
                    );
                }
                else
                {
                    auto texture = texture_cache->GetTexture(texture_path);
                    if(texture->width <= 0)
                    {
                        LOG_WARN
                        (
                            "Failed to load {0} for {1}",
                            texture_src.path,
                            debug_name
                        );
                    }
                    mat.SetTexture(texture_src.type, texture);
                }
            }

            mat.LoadDefaultMaterialsFromShader(texture_cache);

            if(!mat.Validate())
            {
                LOG_WARN
                (
                    "Material {0}({1}) failed validation.",
                    material_src.name,
                    material_index
                );
            }

            ret->materials.push_back(mat);
        }

        const auto material_count = ret->materials.size();

        // todo(Gustav): move this to a data file, load the mesh dynamically
        const auto attributes = std::vector<ShaderAttribute>
        {
            attributes3d::Vertex(),
            attributes3d::Normal(),
            attributes3d::TexCoord()
        };

        for(const auto& part_src: mesh.parts)
        {
            std::shared_ptr<CompiledMeshPart> part {new CompiledMeshPart()};

            PointLayout::Bind(&part->config);
            VertexBuffer::Bind(&part->data);
            IndexBuffer::Bind(&part->tris);

            ConvertPointsToVertexBuffer
            (
                part_src.points,
                attributes,
                &part->data
            );
            BindAttributes(attributes, &part->config);

            ConvertTrisToIndexBuffer(part_src.faces, &part->tris);
            part->tri_count = core::Csizet_to_int(part_src.faces.size());

            IndexBuffer::Bind(nullptr);
            VertexBuffer::Bind(nullptr);
            PointLayout::Bind(nullptr);

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
    CompiledMesh::Render
    (
        const core::mat4f& model_matrix,
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f& camera,
        const Light& light,
        const std::shared_ptr<MaterialOverride>& overridden_materials
    )
    {
        ASSERT
        (
            overridden_materials == nullptr ||
            materials.size() == overridden_materials->materials.size()
        );

        for(const auto& part: parts)
        {
            const auto& material
                = overridden_materials == nullptr
                ? materials[part->material]
                : overridden_materials->materials[part->material];

            material.Apply
            (
                model_matrix,
                projection_matrix,
                view_matrix,
                camera,
                light
            );

            PointLayout::Bind(&part->config);
            IndexBuffer::Bind(&part->tris);
            part->tris.Draw(RenderMode::Triangles, part->tri_count);
            IndexBuffer::Bind(nullptr);
            PointLayout::Bind(nullptr);
        }
    }

}
