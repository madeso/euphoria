#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include <memory>
#include <string>

#include "core/memorychunk.h"

namespace debug
{
  void MemoryChunkToFile(std::shared_ptr<MemoryChunk> chunk, const std::string& file);
}

#endif  // CORE_DEBUG_H
