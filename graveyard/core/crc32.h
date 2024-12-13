#pragma once


namespace eu
{
    struct MemoryChunk;
}

namespace eu::core
{
    unsigned long
    calc_crc32(const MemoryChunk& chunk);

}
