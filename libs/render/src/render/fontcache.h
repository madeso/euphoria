#pragma once

#include <memory>
#include <string>

#include "core/noncopyable.h"


namespace euphoria::core::vfs
{
    struct file_system;
    struct file_path;
}


namespace euphoria::render
{
    struct Font;
    struct TextureCache;

    struct FontCache
    {
        FontCache(core::vfs::file_system* fs, TextureCache* cache);
        ~FontCache();

        NONCOPYABLE(FontCache);

        [[nodiscard]] std::shared_ptr<Font>
        GetFont(const core::vfs::file_path& path) const;

        struct FontCachePimpl;
        std::unique_ptr<FontCachePimpl> pimp;
    };
}
