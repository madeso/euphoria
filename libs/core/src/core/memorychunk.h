#pragma once

#include <memory>


namespace eu::core
{
    struct MemoryChunk
    {
        [[nodiscard]] static std::shared_ptr<MemoryChunk> allocate(int size);
        [[nodiscard]] static std::shared_ptr<MemoryChunk> create_null();

        [[nodiscard]] char* get_data();

        [[nodiscard]] const char* get_data() const;
        [[nodiscard]] int get_size() const;

        char operator[](int index) const;
        char& operator[](int index);

    private:
        std::unique_ptr<char[]> data;
        int size;

        explicit MemoryChunk(int the_size);
    };

    void copy_to_memory(MemoryChunk* memory, const void* src);

    [[nodiscard]] std::shared_ptr<MemoryChunk> create_memory_chunk_from_string(const std::string& content);

    struct MemoryChunkFile
    {
        std::shared_ptr<MemoryChunk> data;
        int position;

        explicit MemoryChunkFile(std::shared_ptr<MemoryChunk> d);

        void write(const void* src, int size);
    };

}
