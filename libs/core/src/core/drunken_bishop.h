#ifndef EUPHORIA_CORE_DRUNKEN_BISHOP_H
#define EUPHORIA_CORE_DRUNKEN_BISHOP_H

#include <vector>

#include "core/table.h"
#include "core/ints.h"


namespace euphoria::core
{
    // split hash to a msb->lsb array
    std::vector<u8>
    to_bytes(u32 hash);

    // split hash to a msb->lsb array
    std::vector<u8>
    to_bytes(u64 hash);

    // split to 2-bit ints
    std::vector<int>
    to_codes(u8 byte, bool msb_first);

    std::vector<int>
    to_codes(const std::vector<u8>& bytes, bool msb_first);

    table<int>
    drunken_bishop
    (
        u32 hash,
        int width,
        int height,
        bool msb_first = true,
        int startx = -1,
        int starty = -1
    );

    table<int>
    drunken_bishop
    (
        u64 hash,
        int width,
        int height,
        bool msb_first = true,
        int startx = -1,
        int starty = -1
    );

    table<int>
    drunken_bishop
    (
        const std::vector<int>& codes,
        int width,
        int height,
        int startx = -1,
        int starty = -1
    );


    std::vector<std::string>
    get_ssh_characters();

    std::vector<std::string>
    collapse
    (
        const table<int>& table,
        const std::vector<std::string>& characters
    );

    // todo(Gustav): add image collapse with palette
}

#endif  // EUPHORIA_CORE_DRUNKEN_BISHOP_H

