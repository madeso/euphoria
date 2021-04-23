#ifndef EUPHORIA_CORE_DEFAULT_PARSE_H
#define EUPHORIA_CORE_DEFAULT_PARSE_H


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
        std::enable_if_t<custom_argparser<T>::value != 0, int> = 0
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
            std::is_enum<T>::value == false && custom_argparser<T>::value == 0,
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
            return Result<T>::False();
        }
        else
        {
            return Result<T>::True(t);
        }
    }

    // default parse function for enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == true && custom_argparser<T>::value == 0,
            int
        > = 0
    >
    Result<T>
    default_parse_function
    (
        const std::string& value
    )
    {
        auto matches = core::StringToEnum<T>(value);
        if (matches.single_match)
        {
            return Result<T>::True(matches.values[0]);
        }
        else
        {
            return Result<T>::False(Str() << "did you mean " <<
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
        const std::string r = Str() << "can be either " <<
            quote_and_combine_english_or
            (
                EnumToString<T>()
            );

        return r;
    }

    template
    <
        typename T,
        std::enable_if_t<custom_argparser<T>::value != 0, int> = 0
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
            std::is_enum<T>::value == false && custom_argparser<T>::value == 0,
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
            std::is_enum<T>::value == true && custom_argparser<T>::value == 0,
            int
        > = 0
    >
    std::string
    default_value_to_string(const T& t)
    {
        return core::EnumToString(t);
    }
}


#endif  // EUPHORIA_CORE_DEFAULT_PARSE_H

