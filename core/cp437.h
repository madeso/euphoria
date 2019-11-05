#ifndef EUPHORIA_CORE_CP437_H
#define EUPHORIA_CORE_CP437_H

#include <string_view>

// Code page 437 is the character set of the original IBM PC (personal computer)

namespace euphoria::core
{
    struct Random;

    std::string_view
    GetCp437(int row, int column);

    std::string_view
    GetRandomCp437InUtf8(Random* random);
}

#endif  // EUPHORIA_CORE_CP437_H
