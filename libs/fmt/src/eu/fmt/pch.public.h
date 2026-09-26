// public header file for the precompiled header

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

// todo(Gustav): add macros should use EU_FMT prefix

// ----------------------------------------------------------------------------
// Format forwarder for classes
#define ADD_DEFAULT_FORMATTER(TYPE, STRING, TO_STRING) \
template <> struct fmt::formatter<TYPE>: fmt::formatter<STRING> \
{ \
    template <typename FormatContext> \
    auto format(const TYPE& c, FormatContext& ctx) const \
    { \
        return fmt::formatter<STRING>::format(TO_STRING(c), ctx); \
    } \
}

// ----------------------------------------------------------------------------
// Format enum classes
#define EU_FMT_ENUM_BEGIN(T, DEF) \
template <> struct fmt::formatter<T> : fmt::formatter<std::string_view> {\
    template <typename FormatContext>\
    auto format(T c, FormatContext& ctx) const {\
        using EnumType = T;\
        std::string_view name = DEF;\
        switch (c) {
#define EU_FMT_ENUM_VAL(V)\
        case EnumType::V:   name = #V; break
#define EU_FMT_ENUM_END()\
        }\
        return formatter<string_view>::format(name, ctx);\
    }\
};


// ----------------------------------------------------------------------------
// catch formatter

#define ADD_CATCH_FORMATTER_DEF(TYPE) \
std::ostream& \
operator<<(std::ostream& stream, const TYPE& v);

#define ADD_CATCH_FORMATTER_IMPL(TYPE) \
std::ostream& \
operator<<(std::ostream& stream, const TYPE& v) \
{ \
    return stream << ::eu::string_from(v); \
}
