#include "core/crc32.h"

#include "base/memorychunk.h"

#include "crc32/crc32.h"


namespace eu::core
{
    unsigned long
    calc_crc32(const MemoryChunk& chunk)
    {
        return Crc32_ComputeBuf(0, chunk.get_data(), chunk.get_size());
    }
}
