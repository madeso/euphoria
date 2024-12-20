#pragma once

#include <memory>
#include <optional>


namespace eu::io
{
    struct FileSystem;
    struct FilePath;
}

namespace eu::render
{
    struct Texture2;

    struct TextureCache
    {
        explicit TextureCache(io::FileSystem* fs);
        ~TextureCache();

        TextureCache(const TextureCache& other) = delete;
        void operator=(const TextureCache&) = delete;
        TextureCache(TextureCache&& other) = delete;
        void operator=(TextureCache&&) = delete;

        [[nodiscard]] std::shared_ptr<Texture2>
        get_texture(const io::FilePath& path) const;

        [[nodiscard]] std::shared_ptr<Texture2>
        get_texture(const std::optional<io::FilePath>& path) const;

        struct TextureCachePimpl;
        std::unique_ptr<TextureCachePimpl> pimpl;
    };
}
