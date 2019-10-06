#ifndef CORE_CRC32_H
#define CORE_CRC32_H

namespace euphoria::core
{
    class MemoryChunk;

    unsigned long
    Crc32(const MemoryChunk& chunk);

}  // namespace euphoria::core

#endif  // CORE_CRC32_H
