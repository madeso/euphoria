#ifndef GUI_ROOT_H
#define GUI_ROOT_H

#include "gui/uistate.h"
#include "gui/layoutcontainer.h"
#include "core/vec2.h"

#include <string>
#include <vector>
#include <memory>


namespace euphoria::core
{
  namespace vfs
  {
    class FileSystem;
  }
}

namespace euphoria::render
{
  class Texture2d;
  class FontCache;
  class TextureCache;
}

namespace euphoria::gui
{
  class Skin;
  class Root
  {
  public:
    Root(const core::Sizef& size);
    ~Root();

    bool
    Load(
        core::vfs::FileSystem*        fs,
        render::FontCache*         font,
        const std::string& path,
        render::TextureCache*      cache);

    void
    SetInputMouse(const core::vec2f& pos, bool down);
    
    void
    Step(float dt);

    void
    Render(render::SpriteRenderer* sp) const;

    std::vector<std::shared_ptr<Skin>> skins_;
    core::Sizef                              size_;
    UiState                            state_;
    LayoutContainer                    container_;
    std::shared_ptr<render::Texture2d>         cursor_image;
    std::shared_ptr<render::Texture2d>         hover_image;
  };
}

#endif  // GUI_ROOT_H
