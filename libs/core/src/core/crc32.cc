#include "core/crc32.h"
#include "core/memorychunk.h"

#include "crc32/crc32.h"


namespace euphoria::core
{
    unsigned long
    calculate_crc32(const memory_chunk& chunk)
    {
        return Crc32_ComputeBuf(0, chunk.get_data(), chunk.get_size());
    }
}
