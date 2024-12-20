#pragma once



#include "base/result.h"


namespace eu
{
    template<typename T>
    struct StringParser
    {
        enum { value = 0 };

        // add a template specialization for the custom type with value=1

        // static Result<T> parse(const std::string& value)
        // static std::string to_string(const T&)
    };

    template<typename T>
    std::string
    to_string_for_custom_argparser(const T& t)
    {
        return StringParser<T>::to_string(t);
    }

    template<typename T>
    Result<T>
    parse_for_custom_argparser(const std::string& value)
    {
        return StringParser<T>::parse(value);
    }
}
