#include "core/memorychunk.h"

#include <cstring> // for memcpy
#include <utility>

#include "core/assert.h"
#include "core/cint.h"

namespace euphoria::core
{
    char*
    memory_chunk::get_data()
    {
        return data.get();
    }

    const char*
    memory_chunk::get_data() const
    {
        return data.get();
    }

    int
    memory_chunk::get_size() const
    {
        return size;
    }

    char memory_chunk::operator[](int index) const
    {
        return data[index];
    }

    char& memory_chunk::operator[](int index)
    {
        return data[index];
    }

    std::shared_ptr<memory_chunk>
    memory_chunk::allocate(int size)
    {
        std::shared_ptr<memory_chunk> ret {new memory_chunk(size)};
        return ret;
    }

    std::shared_ptr<memory_chunk>
    memory_chunk::null()
    {
        std::shared_ptr<memory_chunk> ret;
        return ret;
    }

    memory_chunk::memory_chunk(int the_size) : size(the_size)
    {
        ASSERT(size > 0);
        data = std::make_unique<char[]>(size);
    }

    void
    copy_to_memory(memory_chunk* memory, const void* src)
    {
        std::memcpy(memory->get_data(), src, sizeof(char) * memory->get_size());
    }

    std::shared_ptr<memory_chunk>
    create_memory_chunk_from_string(const std::string& content)
    {
        auto file = memory_chunk::allocate
        (
                c_sizet_to_int(content.length() + 1)
        );
        copy_to_memory(file.get(), &content[0]);
        return file;
    }

    memory_chunk_file::memory_chunk_file(std::shared_ptr<memory_chunk> d)
        : data(std::move(d))
        , position(0)
    {
    }

    void
    memory_chunk_file::write(const void* src, int size)
    {
        ASSERT(position + size <= data->get_size());
        std::memcpy(data->get_data() + position, src, sizeof(char) * size);
        position += size;
    }

}
