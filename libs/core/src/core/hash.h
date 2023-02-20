#pragma once

#include <string_view>

#include "core/ints.h"

namespace euphoria::core
{
    // implements fnv-1a: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

    constexpr U64 hash64(const std::string_view str, U64 hash = 0xcbf29ce484222325)
    {
        return str.empty() ? hash : hash64(str.substr(1), (hash ^ str[0]) * 0x100000001b3);
    }
}
