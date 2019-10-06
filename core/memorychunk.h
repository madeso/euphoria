#ifndef EUPHORIA_MEMORYCHUNK_H
#define EUPHORIA_MEMORYCHUNK_H

#include <memory>
#include <string>

#include "core/ints.h"

namespace euphoria::core
{
    class MemoryChunk
    {
        public:
        char*
        GetData();

        const char*
        GetData() const;

        fuint64
        GetSize() const;

        char  operator[](fuint64 index) const;
        char& operator[](fuint64 index);

        static std::shared_ptr<MemoryChunk>
        Alloc(fuint64 size);

        static std::shared_ptr<MemoryChunk>
        Null();

        private:
        explicit MemoryChunk(fuint64 size);
        std::unique_ptr<char[]> data_;
        fuint64                 size_;
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
        Write(const void* src, fuint64 size);

        std::shared_ptr<MemoryChunk> data;
        fuint64                      position;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_MEMORYCHUNK_H
