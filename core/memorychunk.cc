#include "core/memorychunk.h"
#include "core/assert.h"
#include <cstring> // for memcpy

unsigned char* MemoryChunk::GetData()
{
  return data_.get();
}

const unsigned char* MemoryChunk::GetData() const
{
  return data_.get();
}

unsigned long MemoryChunk::GetSize() const
{
  return size_;
}

std::shared_ptr<MemoryChunk> MemoryChunk::Alloc(unsigned long size) {
  std::shared_ptr<MemoryChunk> ret { new MemoryChunk(size) };
  return ret;
}

std::shared_ptr<MemoryChunk> MemoryChunk::Null() {
  std::shared_ptr<MemoryChunk> ret;
  return ret;
}

MemoryChunk::MemoryChunk(unsigned long size) : size_(size){
  Assert(size > 0);
  data_.reset(new unsigned char[size]);
}

void CopyToMemory(MemoryChunk* file, const void* src) {
  std::memcpy(file->GetData(), src, sizeof(unsigned char) * file->GetSize());
}

MemoryChunkFile::MemoryChunkFile(std::shared_ptr<MemoryChunk> d)
: data(d), position(0)
{}

void MemoryChunkFile::Write(void* src, unsigned long size)
{
  Assert(position + size <= data->GetSize());
  std::memcpy(data->GetData() + position, src, sizeof(unsigned char) * size);
  position += size;
}