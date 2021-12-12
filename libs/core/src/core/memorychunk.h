#pragma once

#include <memory>
#include <string>

namespace euphoria::core
{
    struct MemoryChunk
    {
        [[nodiscard]] char*
        get_data();

        [[nodiscard]] const char*
        get_data() const;

        [[nodiscard]] int
        get_size() const;

        char operator[](int index) const;
        char& operator[](int index);

        [[nodiscard]] static std::shared_ptr<MemoryChunk>
        allocate(int size);

        [[nodiscard]] static std::shared_ptr<MemoryChunk>
        null();

    private:
        explicit MemoryChunk(int the_size);
        std::unique_ptr<char[]> data;
        int size;
    };

    void
    copy_to_memory(MemoryChunk* memory, const void* src);

    [[nodiscard]] std::shared_ptr<MemoryChunk>
    create_memory_chunk_from_string(const std::string& content);

    struct MemoryChunkFile
    {
        explicit MemoryChunkFile(std::shared_ptr<MemoryChunk> d);

        void
        write(const void* src, int size);

        std::shared_ptr<MemoryChunk> data;
        int position;
    };

}
