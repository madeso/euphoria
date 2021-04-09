#include "core/io.h"

#include "core/log.h"

#include <fstream>


namespace euphoria::core::io
{
    void
    ChunkToFile
    (
        std::shared_ptr<MemoryChunk> chunk,
        const std::string& full_path
    )
    {
        std::ofstream file_handle(full_path, std::ios::binary | std::ios::out);
        if(!file_handle.good())
        {
            LOG_ERROR("Failed to open file for writing: {0}", full_path);
            return;
        }
        file_handle.write(chunk->GetData(), chunk->GetSize());
    }

    std::shared_ptr<MemoryChunk>
    FileToChunk(const std::string& full_path)
    {
        std::ifstream is(full_path, std::ifstream::binary);
        if(!is)
        {
            // this is actually not a error since other files might be later on in the vfs
            // test running test3d for a example where it tries to load real files over virtual ones
            // LOG_ERROR("Failed to read real file " << full_path);
            return MemoryChunk::Null();
        }

        is.seekg(0, std::ifstream::end);
        const auto length = is.tellg();
        is.seekg(0, std::ifstream::beg);

        if(length <= 0)
        {
            return MemoryChunk::Null();
        }

        auto memory = MemoryChunk::Alloc(length);
        is.read
        (
            static_cast<char*>(static_cast<void*>(memory->GetData())),
            memory->GetSize()
        );

        return memory;
    }
}
