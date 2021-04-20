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

        // std::string ToString(const T&)
        // Result<T> Parse(const std::string& value)
    };

    template<typename T>
    std::string
    CustomArgparser_ToString(const T& t)
    {
        return custom_argparser<T>::ToString(t);
    }

    template<typename T>
    Result<T>
    CustomArgparser_Parse(const std::string& value)
    {
        return custom_argparser<T>::Parse(value);
    }
}

#endif  // EUPHORIA_CORE_CUSTOM_PARSER_H
