#pragma once

#include "base/ints.h"

#include <string_view>
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
struct Hsh
{
    /// The actual hash. Prefer to use operators on \ref Hsh instead of accessing this directly.
    const u64 hash;

    #if USE_HASH_TEXT == 1
    const std::string_view text;
    #endif

    /// Creates a new hash object and computes the hash at compile time if possible.
    constexpr Hsh(const std::string_view s)
        : hash( hash64(s) )
        #if USE_HASH_TEXT == 1
        , text(s)
        #endif
          { }
};

#define OP(op) \
    constexpr bool operator op(const Hsh& lhs, const Hsh& rhs)\
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

}

namespace std
{

template <> struct hash<eu::Hsh>
{
    std::size_t operator()(const eu::Hsh& x) const
        { return x.hash; }
};

}
