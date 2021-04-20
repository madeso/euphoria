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

    struct TextureCache
    {
        explicit TextureCache(core::vfs::FileSystem* fs);
        ~TextureCache();

        NONCOPYABLE(TextureCache);

        [[nodiscard]] std::shared_ptr<texture2d>
        GetTexture(const core::vfs::FilePath& path) const;

        [[nodiscard]] std::shared_ptr<texture2d>
        GetTexture(const std::optional<core::vfs::FilePath>& path) const;

        struct TextureCachePimpl;
        std::unique_ptr<TextureCachePimpl> pimp_;
    };
}
