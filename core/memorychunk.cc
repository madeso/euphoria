#include "core/memorychunk.h"
#include "core/assert.h"
#include <cstring>  // for memcpy
#include <utility>

unsigned char*
MemoryChunk::GetData()
{
  return data_.get();
}

const unsigned char*
MemoryChunk::GetData() const
{
  return data_.get();
}

unsigned long
MemoryChunk::GetSize() const
{
  return size_;
}

std::shared_ptr<MemoryChunk>
MemoryChunk::Alloc(unsigned long size)
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

MemoryChunk::MemoryChunk(unsigned long size)
    : size_(size)
{
  Assert(size > 0);
  data_ = std::make_unique<unsigned char[]>(size);
}

void
CopyToMemory(MemoryChunk* memory, const void* src)
{
  std::memcpy(memory->GetData(), src,
              sizeof(unsigned char) * memory->GetSize());
}

MemoryChunkFile::MemoryChunkFile(std::shared_ptr<MemoryChunk> d)
    : data(std::move(d))
    , position(0)
{
}

void
MemoryChunkFile::Write(void* src, unsigned long size)
{
  Assert(position + size <= data->GetSize());
  std::memcpy(data->GetData() + position, src, sizeof(unsigned char) * size);
  position += size;
}
