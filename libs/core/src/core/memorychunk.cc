#include "core/memorychunk.h"

#include <cstring>  // for memcpy
#include <utility>

#include "core/assert.h"
#include "core/cint.h"

namespace euphoria::core
{
    char*
    MemoryChunk::GetData()
    {
        return data_.get();
    }

    const char*
    MemoryChunk::GetData() const
    {
        return data_.get();
    }

    int
    MemoryChunk::GetSize() const
    {
        return size_;
    }

    char MemoryChunk::operator[](int index) const
    {
        return data_[index];
    }

    char& MemoryChunk::operator[](int index)
    {
        return data_[index];
    }

    std::shared_ptr<MemoryChunk>
    MemoryChunk::Alloc(int size)
    {
        std::shared_ptr<MemoryChunk> ret {new MemoryChunk(size)};
        return ret;
    }

    std::shared_ptr<MemoryChunk>
    MemoryChunk::Null()
    {
        std::shared_ptr<MemoryChunk> ret;
        return ret;
    }

    MemoryChunk::MemoryChunk(int size) : size_(size)
    {
        ASSERT(size > 0);
        data_ = std::make_unique<char[]>(size);
    }

    void
    CopyToMemory(MemoryChunk* memory, const void* src)
    {
        std::memcpy(memory->GetData(), src, sizeof(char) * memory->GetSize());
    }

    std::shared_ptr<MemoryChunk>
    MemoryChunkFromText(const std::string& content)
    {
        std::shared_ptr<MemoryChunk> file = MemoryChunk::Alloc
        (
            Csizet_to_int(content.length() + 1)
        );
        CopyToMemory(file.get(), &content[0]);
        return file;
    }

    MemoryChunkFile::MemoryChunkFile(std::shared_ptr<MemoryChunk> d)
        : data(std::move(d)), position(0)
    {}

    void
    MemoryChunkFile::Write(const void* src, int size)
    {
        ASSERT(position + size <= data->GetSize());
        std::memcpy(data->GetData() + position, src, sizeof(char) * size);
        position += size;
    }

}  // namespace euphoria::core
