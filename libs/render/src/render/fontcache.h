#pragma once

#include <memory>


namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}


namespace euphoria::render
{
    struct DrawableFont;
    struct TextureCache;

    struct FontCache
    {
        struct FontCachePimpl;
        std::unique_ptr<FontCachePimpl> pimp;

        FontCache(core::vfs::FileSystem* fs, TextureCache* cache);
        ~FontCache();

        FontCache(const FontCache&) = delete;
        FontCache(FontCache&&) = delete;
        void operator=(const FontCache&) = delete;
        void operator=(FontCache&&) = delete;

        [[nodiscard]] std::shared_ptr<DrawableFont> get_font
            (const core::vfs::FilePath& path) const;
    };
}
