#pragma once


#include <memory>
#include <string>

#include "core/memorychunk.h"

namespace euphoria::core::io
{
    void
    chunk_to_file
    (
        std::shared_ptr<memory_chunk> chunk,
        const std::string& full_path
    );

    std::shared_ptr<memory_chunk>
    file_to_chunk(const std::string& full_path);
}
