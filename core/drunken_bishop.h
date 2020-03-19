#ifndef EUPHORIA_CORE_DRUNKEN_BISHOP_H
#define EUPHORIA_CORE_DRUNKEN_BISHOP_H

#include <vector>

#include "core/table.h"
#include "core/ints.h"


namespace euphoria::core
{
    // split hash to a msb->lsb array
    std::vector<u8>
    ToBytes(u32 hash);

    // split hash to a msb->lsb array
    std::vector<u8>
    ToBytes(u64 hash);

    // split to 2-bit ints
    std::vector<int>
    ToCodes(u8 byte);

    Table<int>
    DrunkenBishop
    (
        u32 hash,
        int width,
        int height,
        int startx = -1,
        int starty = -1
    );

    Table<int>
    DrunkenBishop
    (
        u64 hash,
        int width,
        int height,
        int startx = -1,
        int starty = -1
    );

    Table<int>
    DrunkenBishop
    (
        const std::vector<int>& codes,
        int width,
        int height,
        int startx = -1,
        int starty = -1
    );


    std::vector<std::string>
    GetSshCharacters();

    std::string
    Collapse
    (
        const Table<int>& table,
        const std::vector<std::string> characters
    );

    // todo: add image collapse with palette
}

#endif  // EUPHORIA_CORE_DRUNKEN_BISHOP_H

