#pragma once

#include <memory>

#include <optional>

#include "core/noncopyable.h"

namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace euphoria::render
{
    struct Texture2;

    struct TextureCache
    {
        explicit TextureCache(core::vfs::FileSystem* fs);
        ~TextureCache();

        NONCOPYABLE(TextureCache);

        [[nodiscard]] std::shared_ptr<Texture2>
        get_texture(const core::vfs::FilePath& path) const;

        [[nodiscard]] std::shared_ptr<Texture2>
        get_texture(const std::optional<core::vfs::FilePath>& path) const;

        struct TextureCachePimpl;
        std::unique_ptr<TextureCachePimpl> pimpl;
    };
}
