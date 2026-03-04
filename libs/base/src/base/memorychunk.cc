#include "base/memorychunk.h"

#include <fstream>

namespace eu
{

MemoryChunkData::operator MemoryChunk() const
{
    return {.bytes = bytes.data(), .size = bytes.size() };
}



MemoryChunkData chunk_from_file(const std::string& full_path)
{
    std::ifstream is(full_path, std::ifstream::binary);
    if(!is)
    {
        LOG_ERR("Failed to read real file {}", full_path);
        return {};
    }

    is.seekg(0, std::ifstream::end);
    const auto length = is.tellg();
    is.seekg(0, std::ifstream::beg);

    if(length <= 0)
    {
        LOG_ERR("File empty: {}", full_path);
        return {};
    }

    MemoryChunkData memory;
    memory.bytes.resize(length);
    is.read
    (
        memory.bytes.data(),
        length
    );

    return memory;
}

}
