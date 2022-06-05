#pragma once


#include <map>
#include <memory>

#include "core/vec3.h"

namespace euphoria::core
{
    enum class ValueType
    {
        int_type,
        float_type,
        vec3f_type,
        map_type
    };

    struct Value
    {
        explicit Value(ValueType vt) : type(vt) {}

        virtual ~Value() = default;

        Value(const Value&) = delete;
        Value(Value&&) = delete;
        void operator=(const Value&) = delete;
        void operator=(Value&&) = delete;

        const ValueType type;
    };

    struct IntValue : public Value
    {
        explicit IntValue(int i);
        int value;

        [[nodiscard]] static int&
        cast(core::Value* value);
    };

    struct FloatValue : public Value
    {
        explicit FloatValue(float f);
        float value;

        [[nodiscard]] static float&
        cast(core::Value* value);
    };

    struct Vec3fValue : public Value
    {
        explicit Vec3fValue(const Vec3f& v);
        Vec3f value;

        [[nodiscard]] static Vec3f&
        cast(core::Value* value);
    };

    struct MapValue : public Value
    {
        MapValue();

        void
        set(const std::string& name, std::shared_ptr<Value> value);

        std::shared_ptr<Value>
        get_or_null(const std::string& name);

        std::map<std::string, std::shared_ptr<Value>> properties;
    };

}
