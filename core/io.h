#ifndef CORE_IO_H
#define CORE_IO_H

#include <memory>
#include <string>

#include "core/memorychunk.h"

namespace euphoria::core
{

namespace io
{
  void ChunkToFile(std::shared_ptr<MemoryChunk> chunk, const std::string& full_path);
  std::shared_ptr<MemoryChunk> FileToChunk(const std::string& full_path);
}

}

#endif  // CORE_IO_H
