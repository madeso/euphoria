#pragma once

#include <memory>
#include <optional>


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

        TextureCache(const TextureCache& other) = delete;
        void operator=(const TextureCache&) = delete;
        TextureCache(TextureCache&& other) = delete;
        void operator=(TextureCache&&) = delete;

        [[nodiscard]] std::shared_ptr<Texture2>
        get_texture(const core::vfs::FilePath& path) const;

        [[nodiscard]] std::shared_ptr<Texture2>
        get_texture(const std::optional<core::vfs::FilePath>& path) const;

        struct TextureCachePimpl;
        std::unique_ptr<TextureCachePimpl> pimpl;
    };
}
