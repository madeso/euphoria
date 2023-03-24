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
        const ValueType type;

        explicit Value(ValueType vt) : type(vt) {}
        virtual ~Value() = default;

        Value(const Value&) = delete;
        Value(Value&&) = delete;
        void operator=(const Value&) = delete;
        void operator=(Value&&) = delete;
    };

    struct IntValue : public Value
    {
        int value;

        explicit IntValue(int i);

        [[nodiscard]] static int& get(core::Value* value);
    };

    struct FloatValue : public Value
    {
        float value;

        explicit FloatValue(float f);

        [[nodiscard]] static float& get(core::Value* value);
    };

    struct Vec3fValue : public Value
    {
        vec3f value;

        explicit Vec3fValue(const vec3f& v);

        [[nodiscard]] static vec3f& get(core::Value* value);
    };

    struct MapValue : public Value
    {
        std::map<std::string, std::shared_ptr<Value>> properties;

        MapValue();

        void set(const std::string& name, std::shared_ptr<Value> value);
        std::shared_ptr<Value> get_or_null(const std::string& name);
    };

}
