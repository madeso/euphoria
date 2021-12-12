#pragma once

namespace euphoria::core
{
    struct MemoryChunk;

    unsigned long
    calculate_crc32(const MemoryChunk& chunk);

}
