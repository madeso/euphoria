#pragma once


#include <memory>
#include <optional>

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

    std::optional<std::string>
    file_to_string(const std::string& full_path);
}
