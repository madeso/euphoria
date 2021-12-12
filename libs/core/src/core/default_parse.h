#pragma once

#include <string>
#include <vector>

#include "core/enumtostring.h"
#include "core/custom_parser.h"


namespace euphoria::core::argparse
{
    std::string
    quote_and_combine_english_or(const std::vector<std::string>& matches);


    template
    <
        typename T,
        std::enable_if_t<CustomArgparser<T>::value != 0, int> = 0
    >
    Result<T>
    default_parse_function
    (
        const std::string& value
    )
    {
        auto r = custom_argparser_parse<T>(value);
        return r;
    }

    // default parse function for non-enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == false && CustomArgparser<T>::value == 0,
            int
        > = 0
    >
    Result<T>
    default_parse_function
    (
        const std::string& value
    )
    {
        auto stream = std::istringstream{value};
        T t;
        stream >> t;
        if(stream.fail() || !stream.eof())
        {
            return Result<T>::create_error();
        }
        else
        {
            return Result<T>::create_value(t);
        }
    }

    // default parse function for enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == true && CustomArgparser<T>::value == 0,
            int
        > = 0
    >
    Result<T>
    default_parse_function
    (
        const std::string& value
    )
    {
        auto matches = core::string_to_enum<T>(value);
        if (matches.single_match)
        {
            return Result<T>::create_value(matches.values[0]);
        }
        else
        {
            return Result<T>::create_error(StringBuilder() << "did you mean " <<
                quote_and_combine_english_or(matches.names)
                << '?'
            );
        }
    }

    // default describe function for non-enums
    template
    <
        typename T,
        std::enable_if_t<std::is_enum<T>::value == false, int> = 0
    >
    std::optional<std::string>
    default_describe()
    {
        return std::nullopt;
    }

    // default describe function for enums
    template
    <
        typename T,
        std::enable_if_t<std::is_enum<T>::value == true, int> = 0
    >
    std::optional<std::string>
    default_describe()
    {
        const std::string r = StringBuilder() << "can be either " <<
            quote_and_combine_english_or
            (
                enum_to_string<T>()
            );

        return r;
    }

    template
    <
        typename T,
        std::enable_if_t<CustomArgparser<T>::value != 0, int> = 0
    >
    std::string
    default_value_to_string(const T& t)
    {
        return custom_argparser_to_string(t);
    }


    // default value for non enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == false && CustomArgparser<T>::value == 0,
            int
        > = 0
    >
    std::string
    default_value_to_string(const T& t)
    {
        std::ostringstream ss;
        ss << t;
        return ss.str();
    }

    // default value for enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == true && CustomArgparser<T>::value == 0,
            int
        > = 0
    >
    std::string
    default_value_to_string(const T& t)
    {
        return core::enum_to_string(t);
    }
}
