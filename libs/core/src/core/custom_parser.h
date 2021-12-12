#pragma once

#include <string>

#include "core/result.h"


namespace euphoria::core
{
    template<typename T>
    struct CustomArgparser
    {
        enum { value = 0 };

        // add a template specialization for the custom type with value=1

        // static std::string to_string(const T&)
        // static Result<T> parse(const std::string& value)
    };

    template<typename T>
    std::string
    custom_argparser_to_string(const T& t)
    {
        return CustomArgparser<T>::to_string(t);
    }

    template<typename T>
    Result<T>
    custom_argparser_parse(const std::string& value)
    {
        return CustomArgparser<T>::parse(value);
    }
}
