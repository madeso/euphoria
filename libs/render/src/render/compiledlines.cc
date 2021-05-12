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
    ConvertLinesToIndexBuffer(
            const std::vector<core::lines::line>& lines,
            IndexBuffer* buffer)
    {
        std::vector<unsigned int> data;
        data.reserve(lines.size() * 2);
        for(const auto& line: lines)
        {
            const auto [from, to] = line;
            data.emplace_back(from);
            data.emplace_back(to);
        }
        buffer->SetData(data);
    }


    void
    ConvertPointsToVertexBuffer(
            const std::vector<core::line_point>& points,
            const std::vector<ShaderAttribute>& attributes,
            VertexBuffer*                       vb)
    {
        constexpr auto add_float3
                = [](std::vector<float>* dst, const core::vec3f& src) {
                      dst->emplace_back(src.x);
                      dst->emplace_back(src.y);
                      dst->emplace_back(src.z);
                  };
        std::vector<float> data;
        const auto         total_attributes = std::accumulate
        (
            attributes.begin(),
            attributes.end(),
            0,
            [](int count, const ShaderAttribute& att) -> int {
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
                    add_float3(&data, point.point);
                    break;
                case ShaderAttributeSource::Color:
                    ASSERT(att.type == ShaderAttributeType::FLOAT3);
                    add_float3(&data, core::vec3f
                    {
                        point.color.r,
                        point.color.g,
                        point.color.b
                    });
                    break;
                default: DIE("Unhandled case");
                }
            }
        }
        vb->SetData(data);
    }


    void
    CompiledLines::Render
    (
        const core::mat4f& model_matrix,
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f&
    )
    {
        shader->UseShader();

        // set common constants
        shader->SetModel(model_matrix);
        shader->SetProjection(projection_matrix);
        shader->SetView(view_matrix);

        PointLayout::Bind(&config);
        IndexBuffer::Bind(&lines);
        lines.Draw(RenderMode::Lines, line_count);
        IndexBuffer::Bind(nullptr);
        PointLayout::Bind(nullptr);
    }
    

    std::shared_ptr<CompiledLines>
    Compile(MaterialShaderCache* shader_cache, const core::lines& lines)
    {
        std::shared_ptr<CompiledLines> ret {new CompiledLines {}};

        ret->shader = shader_cache->get(core::vfs::file_path{"~/default_line_shader"});

        PointLayout::Bind(&ret->config);
        VertexBuffer::Bind(&ret->data);
        IndexBuffer::Bind(&ret->lines);

        const auto attributes
                = std::vector<ShaderAttribute> {attributes3d::Vertex(),
                                                attributes3d::Color()};

        ConvertPointsToVertexBuffer(
                lines.points, attributes, &ret->data);
        BindAttributes(attributes, &ret->config);

        ConvertLinesToIndexBuffer(lines.indices, &ret->lines);
        ret->line_count = core::Csizet_to_int(lines.indices.size());

        IndexBuffer::Bind(nullptr);
        VertexBuffer::Bind(nullptr);
        PointLayout::Bind(nullptr);

        return ret;
    }
}
