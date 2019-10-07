#ifndef EUPHORIA_MEMORYCHUNK_H
#define EUPHORIA_MEMORYCHUNK_H

#include <memory>
#include <string>

namespace euphoria::core
{
    class MemoryChunk
    {
    public:
        char*
        GetData();

        const char*
        GetData() const;

        int
        GetSize() const;

        char  operator[](int index) const;
        char& operator[](int index);

        static std::shared_ptr<MemoryChunk>
        Alloc(int size);

        static std::shared_ptr<MemoryChunk>
        Null();

    private:
        explicit MemoryChunk(int size);
        std::unique_ptr<char[]> data_;
        int                 size_;
    };

    void
    CopyToMemory(MemoryChunk* memory, const void* src);

    std::shared_ptr<MemoryChunk>
    MemoryChunkFromText(const std::string& content);

    class MemoryChunkFile
    {
    public:
        explicit MemoryChunkFile(std::shared_ptr<MemoryChunk> d);

        void
        Write(const void* src, int size);

        std::shared_ptr<MemoryChunk> data;
        int                      position;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_MEMORYCHUNK_H
