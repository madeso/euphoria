#pragma once

#include <memory>
#include <string>
#include <optional>

#include "core/noncopyable.h"

namespace euphoria::core::vfs
{
    struct file_system;
    struct file_path;
}

namespace euphoria::render
{
    struct texture2d;

    struct TextureCache
    {
        explicit TextureCache(core::vfs::file_system* fs);
        ~TextureCache();

        NONCOPYABLE(TextureCache);

        [[nodiscard]] std::shared_ptr<texture2d>
        GetTexture(const core::vfs::file_path& path) const;

        [[nodiscard]] std::shared_ptr<texture2d>
        GetTexture(const std::optional<core::vfs::file_path>& path) const;

        struct TextureCachePimpl;
        std::unique_ptr<TextureCachePimpl> pimp_;
    };
}
