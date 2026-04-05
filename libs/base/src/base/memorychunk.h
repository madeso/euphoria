#pragma once

namespace eu
{

struct MemoryChunk
{
    const char* bytes;
    std::size_t size;
};

struct MemoryChunkData
{
    std::vector<char> bytes;

    operator MemoryChunk() const;
};

MemoryChunkData chunk_from_file(const std::string& path);

}
