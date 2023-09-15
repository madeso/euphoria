#include "core/crc32.h"
#include "core/memorychunk.h"

#include "crc32/crc32.h"


namespace eu::core
{
    unsigned long
    calc_crc32(const MemoryChunk& chunk)
    {
        return Crc32_ComputeBuf(0, chunk.get_data(), chunk.get_size());
    }
}
