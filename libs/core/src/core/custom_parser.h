#pragma once



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
    to_string_for_custom_argparser(const T& t)
    {
        return CustomArgparser<T>::to_string(t);
    }

    template<typename T>
    Result<T>
    parse_for_custom_argparser(const std::string& value)
    {
        return CustomArgparser<T>::parse(value);
    }
}
