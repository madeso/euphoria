#pragma once

namespace eu::core
{
    struct MemoryChunk;

    unsigned long
    calc_crc32(const MemoryChunk& chunk);

}
