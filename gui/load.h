#ifndef GUI_LOAD_H
#define GUI_LOAD_H

#include <string>
#include <vector>
#include <memory>

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FilePath;
    }
}

namespace euphoria::render
{
    struct TextureCache;
    struct FontCache;
}

namespace euphoria::gui
{
    struct Root;

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

#endif  // GUI_LOAD_H
