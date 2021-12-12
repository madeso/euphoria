#pragma once

#include <string_view>

// Code page 437 is the character set of the original IBM PC (personal computer)

namespace euphoria::core
{
    struct Random;

    std::string_view
    get_cp437(int c);

    std::string_view
    get_random_cp437_in_utf8(Random* random);
}
