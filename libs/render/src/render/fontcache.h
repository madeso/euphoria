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
    struct drawable_font;
    struct texture_cache;

    struct font_cache
    {
        font_cache(core::vfs::FileSystem* fs, texture_cache* cache);
        ~font_cache();

        NONCOPYABLE(font_cache);

        [[nodiscard]] std::shared_ptr<drawable_font>
        get_font(const core::vfs::FilePath& path) const;

        struct font_cache_pimpl;
        std::unique_ptr<font_cache_pimpl> pimp;
    };
}
