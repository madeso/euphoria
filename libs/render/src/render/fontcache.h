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
    struct Font;
    struct TextureCache;

    struct FontCache
    {
        FontCache(core::vfs::FileSystem* fs, TextureCache* cache);
        ~FontCache();

        NONCOPYABLE(FontCache);

        [[nodiscard]] std::shared_ptr<Font>
        GetFont(const core::vfs::FilePath& path) const;

        struct FontCachePimpl;
        std::unique_ptr<FontCachePimpl> pimp;
    };
}
