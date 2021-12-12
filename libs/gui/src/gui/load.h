#pragma once

#include <string>
#include <vector>
#include <memory>

namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace euphoria::render
{
    struct texture_cache;
    struct font_cache;
}

namespace euphoria::gui
{
    struct root;
}


namespace euphoria::gui
{
    bool
    load_gui
    (
            root* root,
            core::vfs::FileSystem* fs,
            render::font_cache* font,
            const core::vfs::FilePath& path,
            render::texture_cache* cache
    );
}
