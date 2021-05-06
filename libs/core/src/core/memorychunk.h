#ifndef EUPHORIA_MEMORYCHUNK_H
#define EUPHORIA_MEMORYCHUNK_H

#include <memory>
#include <string>

namespace euphoria::core
{
    struct memory_chunk
    {
        [[nodiscard]] char*
        get_data();

        [[nodiscard]] const char*
        get_data() const;

        [[nodiscard]] int
        get_size() const;

        char  operator[](int index) const;
        char& operator[](int index);

        [[nodiscard]] static std::shared_ptr<memory_chunk>
        allocate(int size);

        [[nodiscard]] static std::shared_ptr<memory_chunk>
        null();

    private:
        explicit memory_chunk(int size);
        std::unique_ptr<char[]> data_;
        int size_;
    };

    void
    copy_to_memory(memory_chunk* memory, const void* src);

    [[nodiscard]] std::shared_ptr<memory_chunk>
    create_memory_chunk_from_string(const std::string& content);

    struct memory_chunk_file
    {
        explicit memory_chunk_file(std::shared_ptr<memory_chunk> d);

        void
        write(const void* src, int size);

        std::shared_ptr<memory_chunk> data;
        int position;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_MEMORYCHUNK_H
