#pragma once

#include <memory>
#include <string>
#include <optional>

#include "core/noncopyable.h"

namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace euphoria::render
{
    struct texture2d;

    struct texture_cache
    {
        explicit texture_cache(core::vfs::FileSystem* fs);
        ~texture_cache();

        NONCOPYABLE(texture_cache);

        [[nodiscard]] std::shared_ptr<texture2d>
        get_texture(const core::vfs::FilePath& path) const;

        [[nodiscard]] std::shared_ptr<texture2d>
        get_texture(const std::optional<core::vfs::FilePath>& path) const;

        struct texture_cache_pimpl;
        std::unique_ptr<texture_cache_pimpl> pimpl;
    };
}
