#include "core/propertytree.h"

#include "assert/assert.h"


namespace euphoria::core
{
    ////////////////////////////////////////////////////////////////////////////////

    IntValue::IntValue(int i) : core::Value(ValueType::int_type), value(i) {}

    int&
    IntValue::cast(core::Value* value)
    {
        ASSERT(value);
        ASSERT(value->type == ValueType::int_type);
        return static_cast<IntValue*>(value)->value;
    }

    ////////////////////////////////////////////////////////////////////////////////

    FloatValue::FloatValue(float f) : core::Value(ValueType::float_type), value(f) {}

    float&
    FloatValue::cast(core::Value* value)
    {
        ASSERT(value);
        ASSERT(value->type == ValueType::float_type);
        return static_cast<FloatValue*>(value)->value;
    }

    ////////////////////////////////////////////////////////////////////////////////

    Vec3fValue::Vec3fValue(const Vec3f& v) : core::Value(ValueType::vec3f_type), value(v)
    {}

    Vec3f&
    Vec3fValue::cast(core::Value* value)
    {
        ASSERT(value);
        ASSERT(value->type == ValueType::vec3f_type);
        return static_cast<Vec3fValue*>(value)->value;
    }


    ////////////////////////////////////////////////////////////////////////////////

    MapValue::MapValue() : Value(ValueType::map_type) {}

    void
    MapValue::set(const std::string& name, std::shared_ptr<Value> value)
    {
        properties[name] = value;
    }

    std::shared_ptr<Value>
    MapValue::get_or_null(const std::string& name)
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
