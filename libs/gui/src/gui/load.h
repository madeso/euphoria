#pragma once

#include <string>
#include <vector>
#include <memory>

namespace euphoria::core::vfs
{
    struct file_system;
    struct file_path;
}

namespace euphoria::render
{
    struct texture_cache;
    struct font_cache;
}

namespace euphoria::gui
{
    struct Root;
}


namespace euphoria::gui
{
    bool
    Load
    (
        Root* root,
        core::vfs::file_system* fs,
        render::font_cache* font,
        const core::vfs::file_path& path,
        render::texture_cache* cache
    );
}
