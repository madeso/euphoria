#pragma once



#include <memory>

namespace eu::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace eu::render
{
    struct TextureCache;
    struct FontCache;
}

namespace eu::gui
{
    struct Root;
}


namespace eu::gui
{
    bool
    load_gui
    (
            Root* root,
            core::vfs::FileSystem* fs,
            render::FontCache* font,
            const core::vfs::FilePath& path,
            render::TextureCache* cache
    );
}
