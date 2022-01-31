#include "render/compiledmesh.h"

#include <set>
#include <utility>
#include <numeric>

#include "core/cint.h"
#include "core/rgb.h"
#include "core/assert.h"
#include "core/enum.h"
#include "core/texturetypes.h"
#include "log/log.h"
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
    CompiledMeshMaterial::CompiledMeshMaterial()
        : ambient(core::NamedColor::white)
        , diffuse(core::NamedColor::white)
        , specular(core::NamedColor::white)
        , shininess(135.0f)
    {
    }


    void
    CompiledMeshMaterial::set_texture
    (
        const core::EnumValue& name,
        std::shared_ptr<Texture2> texture
    )
    {
        if(textures.find(name) != textures.end())
        {
            LOG_WARN("{0} is already assigned, overwriting...", name.to_string());
        }
        textures[name] = std::move(texture);
    }


    void
    CompiledMeshMaterial::apply
    (
        const core::Mat4f& model_matrix,
        const core::Mat4f& projection_matrix,
        const core::Mat4f& view_matrix,
        const core::Vec3f& camera,
        const Light& light
    ) const
    {
        shader->use_shader();

        // set common constants
        shader->set_model(model_matrix);
        shader->set_projection(projection_matrix);
        shader->set_view(view_matrix);
        shader->setup_light(light, camera);

        shader->set_colors(ambient, diffuse, specular, shininess);

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
            bind_texture_to_shader(
                    texture->second.get(),
                    &shader->shader,
                    binding.uniform,
                    texture_index);
            texture_index += 1;
        }
    }


    void
    CompiledMeshMaterial::load_default_materials_from_shader(TextureCache* cache)
    {
        for(const auto& default_texture: shader->default_textures)
        {
            const bool missing
                    = textures.find(default_texture.name) == textures.end();
            if(missing)
            {
                textures[default_texture.name]
                        = cache->get_texture(default_texture.path);
            }
        }
    }


    bool
    CompiledMeshMaterial::validate() const
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
                    binding.name.to_string()
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
                    name.to_string()
                );
                ok = false;
            }
        }

        return ok;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        DEFINE_ENUM_VALUE(core::texture_type, DiffuseType, "Diffuse"); // NOLINT
    }


    void
    convert_points_to_vertex_buffer
    (
            const std::vector<core::MeshPoint>& points,
            const std::vector<ShaderAttribute>& attributes,
            VertexBuffer* vb
    )
    {
        constexpr auto add_float2 = [](std::vector<float>* dst, const core::Vec2f& src)
        {
            dst->emplace_back(src.x);
            dst->emplace_back(src.y);
        };
        constexpr auto add_float3 = [](std::vector<float>* dst, const core::Vec3f& src)
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
                return count + att.get_element_count();
            }
        );
        data.reserve(total_attributes * points.size());
        for(const auto& point: points)
        {
            for(const auto& att: attributes)
            {
                switch(att.source)
                {
                case ShaderAttributeSource::vertex:
                    ASSERT(att.type == ShaderAttributeType::float3);
                    add_float3(&data, point.vertex);
                    break;
                case ShaderAttributeSource::normal:
                    ASSERT(att.type == ShaderAttributeType::float3);
                    add_float3(&data, point.normal);
                    break;
                case ShaderAttributeSource::uv:
                    ASSERT(att.type == ShaderAttributeType::float2);
                    add_float2(&data, point.uv);
                    break;
                default: DIE("Unhandled case");
                }
            }
        }
        vb->set_data(data);
    }


    void
    convert_tris_to_index_buffer(const std::vector<core::MeshFace>& faces, IndexBuffer* b)
    {
        std::vector<unsigned int> data;
        data.reserve(faces.size() * 3);
        for(const auto& f: faces)
        {
            data.emplace_back(core::c_int_to_unsigned_int(f.a));
            data.emplace_back(core::c_int_to_unsigned_int(f.b));
            data.emplace_back(core::c_int_to_unsigned_int(f.c));
        }
        b->set_data(data);
    }

    std::shared_ptr<CompiledMesh>
    compile_mesh
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
            mat.shader = shader_cache->get(shader_name);
            for(const auto& texture_src: material_src.textures)
            {
                const auto texture_path = core::vfs::resolve_relative
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
                    auto texture = texture_cache->get_texture(texture_path);
                    if(texture->width <= 0)
                    {
                        LOG_WARN
                        (
                            "Failed to load {0} for {1}",
                            texture_src.path,
                            debug_name
                        );
                    }
                    mat.set_texture(texture_src.type, texture);
                }
            }

            mat.load_default_materials_from_shader(texture_cache);

            if(!mat.validate())
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
            attributes3d::vertex(),
            attributes3d::normal(),
            attributes3d::tex_coord()
        };

        for(const auto& part_src: mesh.parts)
        {
            std::shared_ptr<CompiledMeshPart> part {new CompiledMeshPart()};

            PointLayout::bind(&part->config);
            VertexBuffer::bind(&part->data);
            IndexBuffer::bind(&part->tris);

            convert_points_to_vertex_buffer
                    (
                            part_src.points,
                            attributes,
                            &part->data
                    );
            bind_attributes(attributes, &part->config);

            convert_tris_to_index_buffer(part_src.faces, &part->tris);
            part->tri_count = core::c_sizet_to_int(part_src.faces.size());

            IndexBuffer::bind(nullptr);
            VertexBuffer::bind(nullptr);
            PointLayout::bind(nullptr);

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
    CompiledMesh::render
    (
        const core::Mat4f& model_matrix,
        const core::Mat4f& projection_matrix,
        const core::Mat4f& view_matrix,
        const core::Vec3f& camera,
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

            material.apply
                    (
                            model_matrix,
                            projection_matrix,
                            view_matrix,
                            camera,
                            light
                    );

            PointLayout::bind(&part->config);
            IndexBuffer::bind(&part->tris);
            part->tris.draw(RenderMode::triangles, part->tri_count);
            IndexBuffer::bind(nullptr);
            PointLayout::bind(nullptr);
        }
    }

}
