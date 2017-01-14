#ifndef GUI_LOAD_H
#define GUI_LOAD_H

#include <string>

class LayoutContainer;
class TextureCache;
class UiState;
class Font;

bool Load(UiState* state, Font* font, LayoutContainer* root, const std::string& path, TextureCache* cache);

#endif  // GUI_LOAD_H
