#ifndef EUPHORIA_MEMORYCHUNK_H
#define EUPHORIA_MEMORYCHUNK_H

#include <memory>

class MemoryChunk {
 public:
  unsigned char* GetData();
  const unsigned char* GetData() const;

  unsigned long GetSize() const;

  static std::shared_ptr<MemoryChunk> Alloc(unsigned long size);
  static std::shared_ptr<MemoryChunk> Null();
 private:
  explicit MemoryChunk(unsigned long size);
  std::unique_ptr<unsigned char[]> data_;
  unsigned long size_;
};

void CopyToMemory(MemoryChunk* memory, const void* src);


#endif //EUPHORIA_MEMORYCHUNK_H
