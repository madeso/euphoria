#ifndef GUI_LOAD_H
#define GUI_LOAD_H

#include <string>
#include <vector>
#include <memory>

class TextureCache;
class FontCache;
namespace vfs
{
class FileSystem;
}
class Root;

bool
Load(
    Root*              root,
    vfs::FileSystem*        fs,
    FontCache*         font,
    const std::string& path,
    TextureCache*      cache);

#endif  // GUI_LOAD_H
