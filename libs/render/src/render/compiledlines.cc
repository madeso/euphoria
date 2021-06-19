#include "render/compiledlines.h"

#include <numeric>

#include "core/lines.h"
#include "core/cint.h"

#include "render/shaderattribute3d.h"
#include "render/attributebinder.h"
#include "render/materialshader.h"
#include "render/materialshadercache.h"

namespace euphoria::render
{
    void
    convert_lines_to_index_buffer
    (
        const std::vector<core::lines::line>& lines,
        index_buffer* buffer
    )
    {
        std::vector<unsigned int> data;
        data.reserve(lines.size() * 2);
        for(const auto& line: lines)
        {
            const auto [from, to] = line;
            data.emplace_back(from);
            data.emplace_back(to);
        }
        buffer->set_data(data);
    }


    void
    convert_points_to_vertex_buffer
    (
        const std::vector<core::line_point>& points,
        const std::vector<shader_attribute>& attributes,
        vertex_buffer* vb
    )
    {
        constexpr auto add_float3 = [](std::vector<float>* dst, const core::vec3f& src)
        {
            dst->emplace_back(src.x);
            dst->emplace_back(src.y);
            dst->emplace_back(src.z);
        };
        std::vector<float> data;
        const auto total_attributes = std::accumulate
        (
            attributes.begin(),
            attributes.end(),
            0,
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
                    add_float3(&data, point.point);
                    break;
                case shader_attribute_source::color:
                    ASSERT(att.type == shader_attribute_type::float3);
                    add_float3
                    (
                        &data, core::vec3f
                        {
                            point.color.r,
                            point.color.g,
                            point.color.b
                        }
                    );
                    break;
                default:
                    DIE("Unhandled case");
                }
            }
        }
        vb->set_data(data);
    }


    void
    compiled_lines::render
    (
        const core::mat4f& model_matrix,
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f &camera
    )
    {
        shader->use_shader();

        // set common constants
        shader->set_model(model_matrix);
        shader->set_projection(projection_matrix);
        shader->set_view(view_matrix);

        point_layout::bind(&config);
        index_buffer::bind(&lines);
        lines.draw(render_mode::lines, line_count);
        index_buffer::bind(nullptr);
        point_layout::bind(nullptr);
    }
    

    std::shared_ptr<compiled_lines>
    compile(material_shader_cache* shader_cache, const core::lines& lines)
    {
        std::shared_ptr<compiled_lines> ret {new compiled_lines {}};

        ret->shader = shader_cache->get(core::vfs::file_path{"~/default_line_shader"});

        point_layout::bind(&ret->config);
        vertex_buffer::bind(&ret->data);
        index_buffer::bind(&ret->lines);

        const auto attributes = std::vector<shader_attribute>
        {
            attributes3d::vertex(),
            attributes3d::color()
        };

        convert_points_to_vertex_buffer(lines.points, attributes, &ret->data);
        bind_attributes(attributes, &ret->config);

        convert_lines_to_index_buffer(lines.indices, &ret->lines);
        ret->line_count = core::c_sizet_to_int(lines.indices.size());

        index_buffer::bind(nullptr);
        vertex_buffer::bind(nullptr);
        point_layout::bind(nullptr);

        return ret;
    }
}
