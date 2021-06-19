#include "render/shaderattribute.h"

#include <utility>

#include "core/assert.h"
#include "core/cint.h"


namespace euphoria::render
{
    shader_attribute::shader_attribute
    (
        glint a_id,
        shader_attribute_type a_type,
        std::string a_name,
        shader_attribute_source a_source,
        bool a_normalize
    )
        : id(a_id)
        , type(a_type)
        , name(std::move(a_name))
        , source(a_source)
        , normalize(a_normalize)
    {
    }


    int
    shader_attribute::get_element_count() const
    {
        switch(type)
        {
        case shader_attribute_type::float1: return 1;
        case shader_attribute_type::float2: return 2;
        case shader_attribute_type::float3: return 3;
        case shader_attribute_type::float4: return 4;
        case shader_attribute_type::float33: return 3 * 3;
        case shader_attribute_type::float44: return 4 * 4;
        default: DIE("Unhandled shader type"); return 0;
        }
    }


    int
    shader_attribute::get_byte_size() const
    {
        switch(type)
        {
        case shader_attribute_type::float1:
        case shader_attribute_type::float2:
        case shader_attribute_type::float3:
        case shader_attribute_type::float4:
        case shader_attribute_type::float33:
        case shader_attribute_type::float44:
            return core::c_sizet_to_int(sizeof(float)) * get_element_count();
        default:
            DIE("Unhandled shader type"); return 0;
        }
    }


    bool
    operator==(const shader_attribute& lhs, const shader_attribute& rhs)
    {
        return
            lhs.name == rhs.name &&
            lhs.id == rhs.id &&
            lhs.type == rhs.type &&
            lhs.normalize == rhs.normalize;
    }
}
