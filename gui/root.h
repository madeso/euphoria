#ifndef GUI_ROOT_H
#define GUI_ROOT_H

#include <string>

#include "gui/uistate.h"
#include "gui/layoutcontainer.h"

class Font;
class TextureCache;

class Root {
 public:
  Root(const Sizef& size);
  bool Load(Font* font, const std::string& path, TextureCache* cache);

  void Step();
  void Render(SpriteRenderer* sp) const ;

 private:
  Sizef size_;
  UiState state_;
  LayoutContainer container_;
};

#endif  // GUI_ROOT_H
