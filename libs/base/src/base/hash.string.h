#pragma once

#include "base/ints.h"

#include <string_view>
#include <string>
#include <unordered_map>

// 1 = include text in hash, 0=don't
// todo(Gustav): move to a config instead
#define USE_HASH_TEXT 1

namespace eu
{

/// fnv-1a hash.
/// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
constexpr u64 hash64(const std::string_view str, u64 hash = 0xcbf29ce484222325)
    { return str.empty() ? hash : hash64(str.substr(1), (hash ^ str[0]) * 0x100000001b3); }

/// Hash of a non-owning string.
/// It will be (pretty) printed as the original string if that is enabled.
/// Treat it as a regular `std::string_view` but without the string functionality.
/// @see \ref hash64
/// @see \ref HshO
struct Hsh
{
    /// The actual hash.
    /// Prefer to use operators on \ref Hsh instead of accessing this directly.
    u64 hash;

    #if USE_HASH_TEXT == 1
    /// Non-owning storage of the printable string, if enabled.
    /// Please ignore if possible and just print as normal.
    std::string_view text;
    #endif

    /// Creates a new hash object and computes the hash at compile time if possible.
    constexpr Hsh(const std::string_view s)
        : hash( hash64(s) )
        #if USE_HASH_TEXT == 1
        , text(s)
        #endif
          { }
};

/// Hash of an owning string.
/// It will be (pretty) printed as the original string if that is enabled.
/// Treat it as a regular `std::string` but without the string functionality.
/// @see \ref hash64
/// @see \ref Hsh
struct HshO
{
    /// The actual hash.
    /// Prefer to use operators on \ref HshO instead of accessing this directly.
    u64 hash;

#if USE_HASH_TEXT == 1
    /// Owning storage of the printable string, if enabled.
    /// Please ignore if possible and just print as normal.
    std::string text;
#endif

    /// Creates a new hash object and computes the hash at compile time if possible.
    constexpr HshO(const std::string& s)
        : hash(hash64(s))
#if USE_HASH_TEXT == 1
        , text(s)
#endif
    { }

    /// Creates a new hash object and computes the hash at compile time if possible.
    constexpr HshO(const std::string_view& s)
        : hash(hash64(s))
#if USE_HASH_TEXT == 1
        , text(s)
#endif
    { }

    /// Copy data from a non-owning hash.
    constexpr HshO(const Hsh& o)
        : hash(o.hash)
#if USE_HASH_TEXT == 1
        , text(o.text)
#endif
    { }
};

#define OP(op) \
    constexpr bool operator op(const Hsh& lhs, const Hsh& rhs)\
        { return lhs.hash op rhs.hash; }\
    constexpr bool operator op(const HshO& lhs, const Hsh& rhs)\
        { return lhs.hash op rhs.hash; }\
    constexpr bool operator op(const Hsh& lhs, const HshO& rhs)\
        { return lhs.hash op rhs.hash; }\
    constexpr bool operator op(const HshO& lhs, const HshO& rhs)\
        { return lhs.hash op rhs.hash; }
    OP(==) OP(!=) OP(<) OP(>) OP(<=) OP(>=)
#undef OP


template<typename TStream>
TStream& operator<<(TStream& s, const Hsh& hash)
{
    #if USE_HASH_TEXT == 1
    s << hash.text;
    #else
    s << hash.hash;
    #endif

    return s;
}

template<typename TStream>
TStream& operator<<(TStream& s, const HshO& hash)
{
#if USE_HASH_TEXT == 1
    s << hash.text;
#else
    s << hash.hash;
#endif

    return s;
}


}

namespace std
{

/// \private
template <> struct hash<eu::Hsh>
{
    std::size_t operator()(const eu::Hsh& x) const
        { return x.hash; }
};

/// \private
template <> struct hash<eu::HshO>
{
    std::size_t operator()(const eu::HshO& x) const
        { return x.hash; }
};

}
