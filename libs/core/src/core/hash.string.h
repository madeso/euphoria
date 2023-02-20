#pragma once

#include <string_view>

#include "core/ints.h"
#include "core/hash.h"

namespace euphoria::core
{
    struct HashedStringView
    {
        const std::string_view text;
        const U64 hash;

        constexpr HashedStringView(const std::string_view str)
            : text(str)
            , hash( hash64(str) )
        {
        }
    };
}
