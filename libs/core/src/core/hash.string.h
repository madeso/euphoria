#pragma once

#include <string_view>

#include "core/ints.h"
#include "core/hash.h"

namespace euphoria::core
{
    struct hashed_string_view
    {
        constexpr hashed_string_view(const std::string_view str)
            : text(str)
            , hash( hash64(str) )
        {
        }

        const std::string_view text;
        const u64 hash;
    };
}
