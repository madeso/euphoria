#include "core/propertytree.h"

#include "core/assert.h"


namespace euphoria::core
{
    ////////////////////////////////////////////////////////////////////////////////

    value_int::value_int(int i) : core::value(value_type::int_type), value(i) {}

    int&
    value_int::cast(core::value* value)
    {
        ASSERT(value);
        ASSERT(value->type == value_type::int_type);
        return static_cast<value_int*>(value)->value;
    }

    ////////////////////////////////////////////////////////////////////////////////

    value_float::value_float(float f) : core::value(value_type::float_type), value(f) {}

    float&
    value_float::cast(core::value* value)
    {
        ASSERT(value);
        ASSERT(value->type == value_type::float_type);
        return static_cast<value_float*>(value)->value;
    }

    ////////////////////////////////////////////////////////////////////////////////

    value_vec3f::value_vec3f(const vec3f& v) : core::value(value_type::vec3f_type), value(v)
    {}

    vec3f&
    value_vec3f::cast(core::value* value)
    {
        ASSERT(value);
        ASSERT(value->type == value_type::vec3f_type);
        return static_cast<value_vec3f*>(value)->value;
    }


    ////////////////////////////////////////////////////////////////////////////////

    property_tree::property_tree() : value(value_type::struct_type) {}

    void
    property_tree::set(const std::string& name, std::shared_ptr<value> value)
    {
        properties[name] = value;
    }

    std::shared_ptr<value>
    property_tree::get_or_null(const std::string& name)
    {
        auto found = properties.find(name);
        if(found == properties.end())
        {
            return nullptr;
        }
        else
        {
            return found->second;
        }
    }
}
