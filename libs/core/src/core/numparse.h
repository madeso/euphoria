#pragma once

#include <optional>

namespace eu::core
{


std::optional<int> locale_parse_int(const std::string& str);
std::optional<float> locale_parse_float(const std::string& str);
std::optional<bool> locale_parse_bool(const std::string& str);



template<typename T>
std::optional<T>
locale_parse_generic(const std::string& str);



#define SPECIALIZE(TYPE, FUN) \
    template<> std::optional<TYPE> \
    locale_parse_generic<TYPE>(const std::string& str);

    SPECIALIZE(int, locale_parse_int)
    SPECIALIZE(float, locale_parse_float)
    SPECIALIZE(bool, locale_parse_bool)
    SPECIALIZE(std::string, string_parser)
#undef SPECIALIZE


}

