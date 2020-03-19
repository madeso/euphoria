#ifndef EUPHORIA_CORE_DRUNKEN_BISHOP_H
#define EUPHORIA_CORE_DRUNKEN_BISHOP_H

#include <vector>

#include "core/table.h"


namespace euphoria::core
{
    Table<int>
    DrunkenBishop
    (
        int hash,
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

