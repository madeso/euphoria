#include "core/memorychunk.h"
#include "core/assert.h"
#include <cstring>  // for memcpy
#include <utility>

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

fuint64
MemoryChunk::GetSize() const
{
  return size_;
}

char MemoryChunk::operator[](fuint64 index) const
{
  return data_[index];
}

char& MemoryChunk::operator[](fuint64 index)
{
  return data_[index];
}

std::shared_ptr<MemoryChunk>
MemoryChunk::Alloc(fuint64 size)
{
  std::shared_ptr<MemoryChunk> ret{new MemoryChunk(size)};
  return ret;
}

std::shared_ptr<MemoryChunk>
MemoryChunk::Null()
{
  std::shared_ptr<MemoryChunk> ret;
  return ret;
}

MemoryChunk::MemoryChunk(fuint64 size)
    : size_(size)
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
  std::shared_ptr<MemoryChunk> file = MemoryChunk::Alloc(content.length() + 1);
  CopyToMemory(file.get(), &content[0]);
  return file;
}

MemoryChunkFile::MemoryChunkFile(std::shared_ptr<MemoryChunk> d)
    : data(std::move(d))
    , position(0)
{
}

void
MemoryChunkFile::Write(const void* src, fuint64 size)
{
  ASSERT(position + size <= data->GetSize());
  std::memcpy(data->GetData() + position, src, sizeof(char) * size);
  position += size;
}
