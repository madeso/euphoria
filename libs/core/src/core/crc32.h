#pragma once

namespace euphoria::core
{
    struct memory_chunk;

    unsigned long
    calculate_crc32(const memory_chunk& chunk);

}
