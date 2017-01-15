#ifndef GUI_ROOT_H
#define GUI_ROOT_H

#include "gui/uistate.h"
#include "gui/layoutcontainer.h"
#include "core/vec2.h"

#include <string>
#include <vector>
#include <memory>

class Font;
class TextureCache;
class TextBackgroundRenderer;
class Skin;

class Root {
 public:
  Root(const Sizef& size);
  ~Root();
  bool Load(Font* font, const std::string& path, TextureCache* cache, TextBackgroundRenderer* br);

  void SetInputMouse(const vec2f& pos, bool down);
  void Step(float dt);
  void Render(SpriteRenderer* sp) const ;

 private:
  std::vector<std::shared_ptr<Skin>> skins_;
  Sizef size_;
  UiState state_;
  LayoutContainer container_;
};

#endif  // GUI_ROOT_H
