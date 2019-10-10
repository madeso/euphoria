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
    }
}  // namespace euphoria::core

namespace euphoria::render
{
    struct TextureCache;
    struct FontCache;
}  // namespace euphoria::render

namespace euphoria::gui
{
    struct Root;

    bool
    Load(Root*                  root,
         core::vfs::FileSystem* fs,
         render::FontCache*     font,
         const std::string&     path,
         render::TextureCache*  cache);
}  // namespace euphoria::gui

#endif  // GUI_LOAD_H
