#include "core/debug.h"

#include <fstream>

namespace debug
{
  void MemoryChunkToFile(std::shared_ptr<MemoryChunk> chunk, const std::string& file)
  {
    {
      std::ofstream file_handle(file, std::ios::binary | std::ios::out);
      file_handle.write(chunk->GetData(), chunk->GetSize());
    }
    int i = 0;
  }

}  // namespace debug
