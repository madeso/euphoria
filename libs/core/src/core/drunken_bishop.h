#pragma once



#include "core/table.h"
#include "core/ints.h"


namespace eu::core
{
    // split hash to a msb->lsb array
    std::vector<U8>
    to_bytes(U32 hash);

    // split hash to a msb->lsb array
    std::vector<U8>
    to_bytes(U64 hash);

    // split to 2-bit ints
    std::vector<int>
    to_codes(U8 byte, bool msb_first);

    std::vector<int>
    to_codes(const std::vector<U8>& bytes, bool msb_first);

    Table<int>
    get_drunken_bishop_result
    (
        U32 hash,
        int width,
        int height,
        bool msb_first = true,
        int startx = -1,
        int starty = -1
    );

    Table<int>
    get_drunken_bishop_result
    (
        U64 hash,
        int width,
        int height,
        bool msb_first = true,
        int startx = -1,
        int starty = -1
    );

    Table<int>
    get_drunken_bishop_result
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
    render_table
    (
        const Table<int>& table,
        const std::vector<std::string>& characters
    );

    // todo(Gustav): add image collapse with palette
}
