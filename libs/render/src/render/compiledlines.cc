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
        const std::vector<core::Lines::FromToIndices>& lines,
        IndexBuffer* buffer
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
        const std::vector<core::LinePoint>& points,
        const std::vector<ShaderAttribute>& attributes,
        VertexBuffer* vb
    )
    {
        constexpr auto add_float3 = [](std::vector<float>* dst, const core::Vec3f& src)
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
                    add_float3(&data, point.point);
                    break;
                case ShaderAttributeSource::color:
                    ASSERT(att.type == ShaderAttributeType::float3);
                    add_float3
                    (
                        &data, core::Vec3f
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
    CompiledLines::render
    (
        const core::Mat4f& model_matrix,
        const core::Mat4f& projection_matrix,
        const core::Mat4f& view_matrix
    )
    {
        shader->use_shader();

        // set common constants
        shader->set_model(model_matrix);
        shader->set_projection(projection_matrix);
        shader->set_view(view_matrix);

        PointLayout::bind(&config);
        IndexBuffer::bind(&lines);
        lines.draw(RenderMode::lines, line_count);
        IndexBuffer::bind(nullptr);
        PointLayout::bind(nullptr);
    }


    std::shared_ptr<CompiledLines>
    compile(MaterialShaderCache* shader_cache, const core::Lines& lines)
    {
        std::shared_ptr<CompiledLines> ret {new CompiledLines {}};

        ret->shader = shader_cache->get(core::vfs::FilePath{"~/default_line_shader"});

        PointLayout::bind(&ret->config);
        VertexBuffer::bind(&ret->data);
        IndexBuffer::bind(&ret->lines);

        const auto attributes = std::vector<ShaderAttribute>
        {
            attributes3d::vertex(),
            attributes3d::color()
        };

        convert_points_to_vertex_buffer(lines.points, attributes, &ret->data);
        bind_attributes(attributes, &ret->config);

        convert_lines_to_index_buffer(lines.indices, &ret->lines);
        ret->line_count = core::c_sizet_to_int(lines.indices.size());

        IndexBuffer::bind(nullptr);
        VertexBuffer::bind(nullptr);
        PointLayout::bind(nullptr);

        return ret;
    }
}
