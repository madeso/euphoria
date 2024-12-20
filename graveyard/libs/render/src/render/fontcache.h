#pragma once

#include <memory>


namespace eu::io
{
    struct FileSystem;
    struct FilePath;
}


namespace eu::render
{
    struct DrawableFont;
    struct TextureCache;

    struct FontCache
    {
        struct FontCachePimpl;
        std::unique_ptr<FontCachePimpl> pimp;

        FontCache(io::FileSystem* fs, TextureCache* cache);
        ~FontCache();

        FontCache(const FontCache&) = delete;
        FontCache(FontCache&&) = delete;
        void operator=(const FontCache&) = delete;
        void operator=(FontCache&&) = delete;

        [[nodiscard]] std::shared_ptr<DrawableFont> get_font
            (const io::FilePath& path) const;
    };
}
