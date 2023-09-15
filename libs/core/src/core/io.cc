#include "core/io.h"

#include "log/log.h"
#include "core/cint.h"

#include <fstream>


namespace eu::core::io
{
    void
    write_chunk_to_file
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
        file_handle.write(chunk->get_data(), chunk->get_size());
    }

    namespace
    {
        int c_postype_to_int(std::ifstream::pos_type p)
        {
            return static_cast<int>(p);
        }
    }

    std::shared_ptr<MemoryChunk>
    read_file_to_chunk(const std::string& full_path)
    {
        std::ifstream is(full_path, std::ifstream::binary);
        if(!is)
        {
            // this is actually not a error since other files might be later on in the vfs
            // test running test3d for a example where it tries to load real files over virtual ones
            // LOG_ERROR("Failed to read real file " << full_path);
            return MemoryChunk::create_null();
        }

        is.seekg(0, std::ifstream::end);
        const auto length = is.tellg();
        is.seekg(0, std::ifstream::beg);

        if(length <= 0)
        {
            return MemoryChunk::create_null();
        }

        auto memory = MemoryChunk::allocate(c_postype_to_int(length));
        is.read
        (
            static_cast<char*>(static_cast<void*>(memory->get_data())),
            memory->get_size()
        );

        return memory;
    }

    std::optional<std::string>
    read_file_to_string(const std::string& full_path)
    {
        std::ifstream in(full_path, std::ios::in | std::ios::binary);
        if (!in) { return std::nullopt; }
            
        std::string contents;
        in.seekg(0, std::ios::end);
        const auto file_size = in.tellg();
        contents.resize(file_size);
        in.seekg(0, std::ios::beg);
        in.read(contents.data(), file_size);
        in.close();
        return(contents);
    }
}
