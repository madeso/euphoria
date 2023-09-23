#pragma once



#include <memory>

namespace eu::io
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
            io::FileSystem* fs,
            render::FontCache* font,
            const io::FilePath& path,
            render::TextureCache* cache
    );
}
