#include "render/shaderattribute.h"

#include <utility>

#include "core/log.h"

namespace euphoria::render
{
    LOG_SPECIFY_DEFAULT_LOGGER("render.shaderattribute")

    ShaderAttribute::ShaderAttribute(
            glint               a_id,
            ShaderAttributeType a_type,
            std::string         a_name,
            ShaderAttributeSource a_source,
            bool a_normalize)
        : id(a_id), type(a_type), name(std::move(a_name)), source(a_source), normalize(a_normalize)
    {}

    int
    ShaderAttribute::GetElementCount() const
    {
        switch(type)
        {
        case ShaderAttributeType::FLOAT1: return 1;
        case ShaderAttributeType::FLOAT2: return 2;
        case ShaderAttributeType::FLOAT3: return 3;
        case ShaderAttributeType::FLOAT4: return 4;
        case ShaderAttributeType::FLOAT33: return 3*3;
        case ShaderAttributeType::FLOAT44: return 4*4;
        default:
            LOG_ERROR("Unhandled shader type");
            return 0;
        }
    }

    int
    ShaderAttribute::GetByteSize() const
    {
        switch(type)
        {
        case ShaderAttributeType::FLOAT1:
        case ShaderAttributeType::FLOAT2:
        case ShaderAttributeType::FLOAT3:
        case ShaderAttributeType::FLOAT4:
        case ShaderAttributeType::FLOAT33:
        case ShaderAttributeType::FLOAT44:
            return sizeof(float) * GetElementCount();
        default:
            LOG_ERROR("Unhandled shader type");
            return 0;
        }
    }

    bool
    operator==(const ShaderAttribute& lhs, const ShaderAttribute& rhs)
    {
        return lhs.name == rhs.name && lhs.id == rhs.id && lhs.type == rhs.type && lhs.normalize == rhs.normalize;
    }

}  // namespace euphoria::render