#pragma once

#include <optional>

namespace euphoria::core
{


std::optional<int> locale_parse_int(const std::string& str);
std::optional<float> locale_parse_float(const std::string& str);
std::optional<bool> locale_parse_bool(const std::string& str);



template<typename T>
std::optional<T>
locale_parse_generic(const std::string& str);



#define SPECIAlIZE(TYPE, FUN) \
    template<> std::optional<TYPE> \
    locale_parse_generic<TYPE>(const std::string& str);

    SPECIAlIZE(int, locale_parse_int)
    SPECIAlIZE(float, locale_parse_float)
    SPECIAlIZE(bool, locale_parse_bool)
    SPECIAlIZE(std::string, string_parser)
#undef SPECIAlIZE


}

