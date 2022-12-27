#include "core/numparse.h"

#include <cstdlib>
#include <limits>

namespace euphoria::core
{


std::optional<int> locale_parse_int(const std::string& str)
{
    char* end = nullptr;
    const auto r = std::strtol(str.c_str(), &end, 10);
    if(end == str.c_str()) { return std::nullopt; }

    // validate to int range
    if(r < static_cast<long>(std::numeric_limits<int>::min())) { return std::nullopt; }
    if(r > static_cast<long>(std::numeric_limits<int>::max())) { return std::nullopt; }

    return static_cast<int>(r);
}



std::optional<float> locale_parse_float(const std::string& str)
{
    char* end = nullptr;
    const auto r = std::strtof(str.c_str(), &end);
    if(end == str.c_str()) { return std::nullopt; }
    return r;
}



std::optional<bool> locale_parse_bool(const std::string& str)
{
    if(str == "true") { return true; }
    if(str == "yes") { return true; }

    if(str == "false") { return false; }
    if(str == "no") { return false; }

    return std::nullopt;
}


#define SPECIAlIZE(TYPE, FUN) \
    template<> std::optional<TYPE> \
    locale_parse_generic<TYPE>(const std::string& str) { return FUN(str); }

SPECIAlIZE(int, locale_parse_int)
SPECIAlIZE(float, locale_parse_float)
SPECIAlIZE(bool, locale_parse_bool)
SPECIAlIZE(std::string, [](const std::string& s) { return s; })
#undef SPECIAlIZE


}

