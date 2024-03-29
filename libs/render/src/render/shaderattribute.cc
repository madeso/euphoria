#include "render/shaderattribute.h"

#include <utility>

#include "assert/assert.h"
#include "base/cint.h"


namespace eu::render
{
    int
    ShaderAttribute::get_element_count() const
    {
        switch(type)
        {
        case ShaderAttributeType::float1: return 1;
        case ShaderAttributeType::float2: return 2;
        case ShaderAttributeType::float3: return 3;
        case ShaderAttributeType::float4: return 4;
        case ShaderAttributeType::float33: return 3 * 3;
        case ShaderAttributeType::float44: return 4 * 4;
        default: DIE("Unhandled shader type"); return 0;
        }
    }


    int
    ShaderAttribute::get_byte_size() const
    {
        switch(type)
        {
        case ShaderAttributeType::float1:
        case ShaderAttributeType::float2:
        case ShaderAttributeType::float3:
        case ShaderAttributeType::float4:
        case ShaderAttributeType::float33:
        case ShaderAttributeType::float44:
            return c_sizet_to_int(sizeof(float)) * get_element_count();
        default:
            DIE("Unhandled shader type"); return 0;
        }
    }


    bool
    operator==(const ShaderAttribute& lhs, const ShaderAttribute& rhs)
    {
        return
            lhs.name == rhs.name &&
            lhs.id == rhs.id &&
            lhs.type == rhs.type &&
            lhs.normalize == rhs.normalize;
    }
}
