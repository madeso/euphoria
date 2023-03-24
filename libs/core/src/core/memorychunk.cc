#include "core/memorychunk.h"

#include <cstring> // for memcpy
#include <utility>

#include "assert/assert.h"
#include "core/cint.h"

namespace euphoria::core
{
    char*
    MemoryChunk::get_data()
    {
        return data.get();
    }

    const char*
    MemoryChunk::get_data() const
    {
        return data.get();
    }

    int
    MemoryChunk::get_size() const
    {
        return size;
    }

    char MemoryChunk::operator[](int index) const
    {
        return data[index];
    }

    char& MemoryChunk::operator[](int index)
    {
        return data[index];
    }

    std::shared_ptr<MemoryChunk>
    MemoryChunk::allocate(int size)
    {
        std::shared_ptr<MemoryChunk> ret {new MemoryChunk(size)};
        return ret;
    }

    std::shared_ptr<MemoryChunk>
    MemoryChunk::create_null()
    {
        std::shared_ptr<MemoryChunk> ret;
        return ret;
    }

    MemoryChunk::MemoryChunk(int the_size) : size(the_size)
    {
        ASSERT(size > 0);
        data = std::make_unique<char[]>(size);
    }

    void
    copy_to_memory(MemoryChunk* memory, const void* src)
    {
        std::memcpy(memory->get_data(), src, sizeof(char) * memory->get_size());
    }

    std::shared_ptr<MemoryChunk>
    create_memory_chunk_from_string(const std::string& content)
    {
        auto file = MemoryChunk::allocate
        (
                c_sizet_to_int(content.length() + 1)
        );
        copy_to_memory(file.get(), &content[0]);
        return file;
    }

    MemoryChunkFile::MemoryChunkFile(std::shared_ptr<MemoryChunk> d)
        : data(std::move(d))
        , position(0)
    {
    }

    void
    MemoryChunkFile::write(const void* src, int size)
    {
        ASSERT(position + size <= data->get_size());
        std::memcpy(data->get_data() + position, src, sizeof(char) * size);
        position += size;
    }

}
