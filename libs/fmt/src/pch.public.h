// public header file for the precompiled header

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

// todo(Gustav): remove this when all formatters are fully ported
// #include <fmt/ostream.h>
// using namespace fmt::literals;


#define ADD_DEFAULT_FORMATTER(TYPE, STRING, TO_STRING) \
template <> struct fmt::formatter<TYPE>: fmt::formatter<STRING> \
{ \
    template <typename FormatContext> \
    auto format(const TYPE& c, FormatContext& ctx) const \
    { \
        return fmt::formatter<STRING>::format(TO_STRING(c), ctx); \
    } \
}

#define ADD_CATCH_FORMATTER(TYPE) \
constexpr std::ostream& \
operator<<(std::ostream& stream, const TYPE& v) \
{ \
    return stream << ::eu::string_from(v); \
}
