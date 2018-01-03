#ifndef GUI_LOAD_H
#define GUI_LOAD_H

#include <string>
#include <vector>
#include <memory>

class LayoutContainer;
class TextureCache;
class FontCache;
class UiState;
class Font;
class TextBackgroundRenderer;
class Skin;
class FileSystem;

bool
Load(
    FileSystem*                         fs,
    UiState*                            state,
    FontCache*                          font,
    LayoutContainer*                    root,
    const std::string&                  path,
    TextureCache*                       cache,
    TextBackgroundRenderer*             br,
    std::vector<std::shared_ptr<Skin>>* skins);

#endif  // GUI_LOAD_H
