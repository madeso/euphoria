#ifndef EUPHORIA_BASE64_H
#define EUPHORIA_BASE64_H

#include "core/memorychunk.h"

#include <string>

namespace base64
{
  std::string
  Encode(std::shared_ptr<MemoryChunk> memory);

  std::shared_ptr<MemoryChunk>
  Decode(const std::string& data);
}

#endif  // EUPHORIA_BASE64_H
