#ifndef EUPHORIA_CORE_DEFAULT_PARSE_H
#define EUPHORIA_CORE_DEFAULT_PARSE_H


#include <string>
#include <vector>

#include "core/enumtostring.h"
#include "core/custom_parser.h"


namespace euphoria::core::argparse
{
    std::string
    QuoteAndCombineEnglishOr(const std::vector<std::string>& matches);


    template
    <
        typename T,
        std::enable_if_t<CustomArgparser<T>::value != 0, int> = 0
    >
    Result<T>
    DefaultParseFunction
    (
        const std::string& value
    )
    {
        auto r = CustomArgparser_Parse<T>(value);
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
    DefaultParseFunction
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
            std::is_enum<T>::value == true && CustomArgparser<T>::value == 0,
            int
        > = 0
    >
    Result<T>
    DefaultParseFunction
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
                QuoteAndCombineEnglishOr(matches.names)
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
    DefaultDescribe()
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
    DefaultDescribe()
    {
        const std::string r = Str() << "can be either " <<
            QuoteAndCombineEnglishOr
            (
                EnumToString<T>()
            );

        return r;
    }

    template
    <
        typename T,
        std::enable_if_t<CustomArgparser<T>::value != 0, int> = 0
    >
    std::string
    DefaultValueToString(const T& t)
    {
        return CustomArgparser_ToString(t);
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
    DefaultValueToString(const T& t)
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
    DefaultValueToString(const T& t)
    {
        return core::EnumToString(t);
    }
}


#endif  // EUPHORIA_CORE_DEFAULT_PARSE_H

