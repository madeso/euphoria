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
    struct TextureCache;
    struct FontCache;
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
        core::vfs::FileSystem* fs,
        render::FontCache* font,
        const core::vfs::FilePath& path,
        render::TextureCache* cache
    );
}
