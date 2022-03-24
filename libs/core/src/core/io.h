#pragma once


#include <memory>
#include <string>

#include "core/memorychunk.h"

namespace euphoria::core::io
{
    void
    chunk_to_file
    (
        std::shared_ptr<MemoryChunk> chunk,
        const std::string& full_path
    );

    std::shared_ptr<MemoryChunk>
    file_to_chunk(const std::string& full_path);
}