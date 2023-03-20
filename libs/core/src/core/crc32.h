#pragma once

namespace euphoria::core
{
    struct MemoryChunk;

    unsigned long
    calc_crc32(const MemoryChunk& chunk);

}
