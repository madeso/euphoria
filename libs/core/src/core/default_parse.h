#pragma once

#include "core/enumtostring.h"
#include "core/custom_parser.h"
#include "core/numparse.h"


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

    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_same<T, std::string>::value
        , int> = 0
    >
    Result<T>
    default_parse_function
    (
        const std::string& value
    )
    {
        return Result<T>::create_value(value);
    }

    // default parse function for non-enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == false && CustomArgparser<T>::value == 0 && std::is_same<T, std::string>::value == false,
            int
        > = 0
    >
    Result<T>
    default_parse_function
    (
        const std::string& value
    )
    {
        auto parsed = locale_parse_generic<T>(value);
        if(parsed)
        {
            return Result<T>::create_value(*parsed);
        }
        else
        {
            return Result<T>::create_error();
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
        return fmt::format("{}", t);
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
