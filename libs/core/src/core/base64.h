#ifndef EUPHORIA_BASE64_H
#define EUPHORIA_BASE64_H

#include "core/memorychunk.h"

#include <string>

namespace euphoria::core::base64
{
    std::string
    encode(std::shared_ptr<memory_chunk> memory);

    std::shared_ptr<memory_chunk>
    decode(const std::string& input);
}

#endif // EUPHORIA_BASE64_H
