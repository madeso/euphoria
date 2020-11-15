#include "core/propertytree.h"

#include "core/assert.h"


namespace euphoria::core
{
    ////////////////////////////////////////////////////////////////////////////////

    ValueInt::ValueInt(int i) : Value(ValueType::Int), value(i) {}

    int&
    ValueInt::Cast(Value* value)
    {
        ASSERT(value);
        ASSERT(value->type == ValueType::Int);
        return static_cast<ValueInt*>(value)->value;
    }

    ////////////////////////////////////////////////////////////////////////////////

    ValueFloat::ValueFloat(float f) : Value(ValueType::Float), value(f) {}

    float&
    ValueFloat::Cast(Value* value)
    {
        ASSERT(value);
        ASSERT(value->type == ValueType::Float);
        return static_cast<ValueFloat*>(value)->value;
    }

    ////////////////////////////////////////////////////////////////////////////////

    ValueVec3f::ValueVec3f(const vec3f& v) : Value(ValueType::Vec3f), value(v)
    {}

    vec3f&
    ValueVec3f::Cast(Value* value)
    {
        ASSERT(value);
        ASSERT(value->type == ValueType::Vec3f);
        return static_cast<ValueVec3f*>(value)->value;
    }


    ////////////////////////////////////////////////////////////////////////////////

    ValuePoint3f::ValuePoint3f(const vec3f& v)
        : Value(ValueType::Point3f), value(v)
    {}

    vec3f&
    ValuePoint3f::Cast(Value* value)
    {
        ASSERT(value);
        ASSERT(value->type == ValueType::Point3f);
        return static_cast<ValuePoint3f*>(value)->value;
    }

    ////////////////////////////////////////////////////////////////////////////////

    PropertyTree::PropertyTree() : Value(ValueType::Struct) {}

    void
    PropertyTree::Set(const std::string& name, std::shared_ptr<Value> value)
    {
        properties[name] = value;
    }

    std::shared_ptr<Value>
    PropertyTree::GetOrNull(const std::string& name)
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

}  // namespace euphoria::core
