#pragma once

#include <string>
#include <map>
#include <memory>

#include "core/vec3.h"

namespace euphoria::core
{
    enum class value_type
    {
        int_type,
        float_type,
        vec3f_type,
        struct_type
    };

    struct value
    {
        explicit value(value_type vt) : type(vt) {}

        virtual ~value() = default;

        value(const value&) = delete;
        value(value&&) = delete;
        void operator=(const value&) = delete;
        void operator=(value&&) = delete;

        const value_type type;
    };

    struct value_int : public value
    {
        explicit value_int(int i);
        int value;

        [[nodiscard]] static int&
        cast(core::value* value);
    };

    struct value_float : public value
    {
        explicit value_float(float f);
        float value;

        [[nodiscard]] static float&
        cast(core::value* value);
    };

    struct value_vec3f : public value
    {
        explicit value_vec3f(const vec3f& v);
        vec3f value;

        [[nodiscard]] static vec3f&
        cast(core::value* value);
    };

    struct property_tree : public value
    {
        property_tree();

        void
        set(const std::string& name, std::shared_ptr<value> value);

        std::shared_ptr<value>
        get_or_null(const std::string& name);

        std::map<std::string, std::shared_ptr<value>> properties;
    };

}
