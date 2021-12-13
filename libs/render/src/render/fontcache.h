#pragma once

#include <memory>
#include <string>

#include "core/noncopyable.h"


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
        FontCache(core::vfs::FileSystem* fs, TextureCache* cache);
        ~FontCache();

        NONCOPYABLE(FontCache);

        [[nodiscard]] std::shared_ptr<DrawableFont>
        get_font(const core::vfs::FilePath& path) const;

        struct font_cache_pimpl;
        std::unique_ptr<font_cache_pimpl> pimp;
    };
}
