#pragma once


#include <memory>
#include <optional>

#include "core/memorychunk.h"

namespace eu::core::io
{
    void
    write_chunk_to_file
    (
        std::shared_ptr<MemoryChunk> chunk,
        const std::string& full_path
    );

    std::shared_ptr<MemoryChunk>
    read_file_to_chunk(const std::string& full_path);

    std::optional<std::string>
    read_file_to_string(const std::string& full_path);
}
