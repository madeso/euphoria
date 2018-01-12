#ifndef GUI_ROOT_H
#define GUI_ROOT_H

#include "gui/uistate.h"
#include "gui/layoutcontainer.h"
#include "core/vec2.h"

#include <string>
#include <vector>
#include <memory>

class FontCache;
class TextureCache;
class Skin;
class FileSystem;
class Texture2d;

class Root
{
 public:
  Root(const Sizef& size);
  ~Root();
  bool
  Load(
      FileSystem*        fs,
      FontCache*         font,
      const std::string& path,
      TextureCache*      cache);

  void
  SetInputMouse(const vec2f& pos, bool down);
  void
  Step(float dt);
  void
  Render(SpriteRenderer* sp) const;

  std::vector<std::shared_ptr<Skin>> skins_;
  Sizef                              size_;
  UiState                            state_;
  LayoutContainer                    container_;
  std::shared_ptr<Texture2d>         cursor_image;
  std::shared_ptr<Texture2d>         hover_image;
};

#endif  // GUI_ROOT_H
