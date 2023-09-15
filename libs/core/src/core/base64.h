#pragma once

#include "core/memorychunk.h"



namespace eu::core::base64
{
    std::string
    encode(std::shared_ptr<MemoryChunk> memory);

    std::shared_ptr<MemoryChunk>
    decode(const std::string& input);
}
