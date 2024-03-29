#pragma once

#include "base/enumtostring.h"
#include "base/string_io.h"
#include "base/numparse.h"


namespace eu
{
    std::string
    add_quotes_and_combine_with_english_or(const std::vector<std::string>& matches);


    template
    <
        typename T,
        std::enable_if_t<StringParser<T>::value != 0, int> = 0
    >
    Result<T>
    default_parse_function
    (
        const std::string& value
    )
    {
        auto r = parse_for_custom_argparser<T>(value);
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
            std::is_enum<T>::value == false && StringParser<T>::value == 0 && std::is_same<T, std::string>::value == false,
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
            return Result<T>::create_error("");
        }
    }

    // default parse function for enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == true && StringParser<T>::value == 0,
            int
        > = 0
    >
    Result<T>
    default_parse_function
    (
        const std::string& value
    )
    {
        auto matches = from_string_to_enum<T>(value);
        if (matches.single_match)
        {
            return Result<T>::create_value(matches.values[0]);
        }
        else
        {
            return Result<T>::create_error(fmt::format("did you mean {}?", add_quotes_and_combine_with_english_or(matches.names)));
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
        const std::string r = fmt::format("can be either {}",
            add_quotes_and_combine_with_english_or
            (
                get_all_names_from_enum<T>()
            )
        );

        return r;
    }

    template
    <
        typename T,
        std::enable_if_t<StringParser<T>::value != 0, int> = 0
    >
    std::string
    from_default_value_to_string(const T& t)
    {
        return to_string_for_custom_argparser(t);
    }


    // default value for non enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == false && StringParser<T>::value == 0,
            int
        > = 0
    >
    std::string
    from_default_value_to_string(const T& t)
    {
        return fmt::format("{}", t);
    }

    // default value for enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == true && StringParser<T>::value == 0,
            int
        > = 0
    >
    std::string
    from_default_value_to_string(const T& t)
    {
        return from_enum_to_string(t);
    }
}
