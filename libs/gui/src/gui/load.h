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
    struct root;
}


namespace euphoria::gui
{
    bool
    load_gui
    (
            root* root,
            core::vfs::FileSystem* fs,
            render::FontCache* font,
            const core::vfs::FilePath& path,
            render::TextureCache* cache
    );
}
