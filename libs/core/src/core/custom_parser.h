#ifndef EUPHORIA_CORE_CUSTOM_PARSER_H
#define EUPHORIA_CORE_CUSTOM_PARSER_H

#include <string>

#include "core/result.h"


namespace euphoria::core
{
    template<typename T>
    struct custom_argparser
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
        return custom_argparser<T>::to_string(t);
    }

    template<typename T>
    result<T>
    custom_argparser_parse(const std::string& value)
    {
        return custom_argparser<T>::parse(value);
    }
}

#endif  // EUPHORIA_CORE_CUSTOM_PARSER_H
