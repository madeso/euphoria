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
    compiled_mesh_material::compiled_mesh_material()
        : ambient(core::color::white)
        , diffuse(core::color::white)
        , specular(core::color::white)
        , shininess(135.0f)
    {
    }


    void
    compiled_mesh_material::set_texture
    (
        const core::enum_value& name,
        std::shared_ptr<texture2d> texture
    )
    {
        if(textures.find(name) != textures.end())
        {
            LOG_WARN("{0} is already assigned, overwriting...", name.to_string());
        }
        textures[name] = std::move(texture);
    }


    void
    compiled_mesh_material::apply
    (
        const core::mat4f& model_matrix,
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f& camera,
        const light& light
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
    compiled_mesh_material::load_default_materials_from_shader(texture_cache* cache)
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
    compiled_mesh_material::validate() const
    {
        std::set<core::enum_value> shader_values;

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
            const std::vector<core::mesh_point>& points,
            const std::vector<shader_attribute>& attributes,
            vertex_buffer* vb
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
            [](int count, const shader_attribute& att) -> int
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
                case shader_attribute_source::vertex:
                    ASSERT(att.type == shader_attribute_type::float3);
                    add_float3(&data, point.vertex);
                    break;
                case shader_attribute_source::normal:
                    ASSERT(att.type == shader_attribute_type::float3);
                    add_float3(&data, point.normal);
                    break;
                case shader_attribute_source::uv:
                    ASSERT(att.type == shader_attribute_type::float2);
                    add_float2(&data, point.uv);
                    break;
                default: DIE("Unhandled case");
                }
            }
        }
        vb->set_data(data);
    }


    void
    convert_tris_to_index_buffer(const std::vector<core::mesh_face>& faces, index_buffer* b)
    {
        std::vector<unsigned int> data;
        data.reserve(faces.size() * 3);
        for(const auto& f: faces)
        {
            data.emplace_back(core::Cint_to_unsigned_int(f.a));
            data.emplace_back(core::Cint_to_unsigned_int(f.b));
            data.emplace_back(core::Cint_to_unsigned_int(f.c));
        }
        b->set_data(data);
    }

    std::shared_ptr<compiled_mesh>
    compile_mesh
    (
            const core::mesh& mesh,
            material_shader_cache* shader_cache,
            texture_cache* texture_cache,
            const core::vfs::dir_path& texture_folder,
            const std::string& debug_name
    )
    {
        std::shared_ptr<compiled_mesh> ret {new compiled_mesh {}};

        // todo(Gustav): add default material if there are 0 materials

        int material_index = 0;

        for(const auto& material_src: mesh.materials)
        {
            material_index += 1;
            compiled_mesh_material mat;
            mat.ambient = material_src.ambient;
            mat.diffuse = material_src.diffuse;
            mat.specular = material_src.specular;
            mat.shininess = material_src.shininess;

            // todo(Gustav): determine a better default shader name
            // perhaps by setting a few default shaders on a "project" and
            // we try to match a shader to the object
            const auto shader_name = material_src.shader.value_or
            (
                core::vfs::file_path("~/default_shader")
            );
            mat.shader = shader_cache->get(shader_name);
            for(const auto& texture_src: material_src.textures)
            {
                const auto texture_path = core::vfs::resolve_relative
                (
                    core::vfs::file_path{texture_src.path},
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
        const auto attributes = std::vector<shader_attribute>
        {
            attributes3d::vertex(),
            attributes3d::normal(),
            attributes3d::tex_coord()
        };

        for(const auto& part_src: mesh.parts)
        {
            std::shared_ptr<compiled_mesh_part> part {new compiled_mesh_part()};

            point_layout::bind(&part->config);
            vertex_buffer::bind(&part->data);
            index_buffer::bind(&part->tris);

            convert_points_to_vertex_buffer
                    (
                            part_src.points,
                            attributes,
                            &part->data
                    );
            bind_attributes(attributes, &part->config);

            convert_tris_to_index_buffer(part_src.faces, &part->tris);
            part->tri_count = core::Csizet_to_int(part_src.faces.size());

            index_buffer::bind(nullptr);
            vertex_buffer::bind(nullptr);
            point_layout::bind(nullptr);

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
    compiled_mesh::render
    (
        const core::mat4f& model_matrix,
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f& camera,
        const light& light,
        const std::shared_ptr<material_override>& overridden_materials
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

            point_layout::bind(&part->config);
            index_buffer::bind(&part->tris);
            part->tris.draw(render_mode::triangles, part->tri_count);
            index_buffer::bind(nullptr);
            point_layout::bind(nullptr);
        }
    }

}
