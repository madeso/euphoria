#ifndef EUPHORIA_SCIMED_H
#define EUPHORIA_SCIMED_H

#include <memory>

#include "core/vec2.h"

#include "window/canvas.h"

#include "gaf_scalingsprite.h"

namespace euphoria::render
{
  class Texture2d;
  class TextureCache;
}

namespace euphoria::editor
{
  struct LineHoverData
  {
    int vertical_index   = -1;
    int horizontal_index = -1;
  };

  struct ScimedConfig
  {
    int          sizer_distance      = 20;
    int          sizer_text_distance = 23;
    int          anchor_size         = 6;
    unsigned int sizer_color         = IM_COL32(0, 0, 0, 255);
    unsigned int split_color         = IM_COL32(0, 255, 0, 255);
  };

  struct Scimed
  {
    std::shared_ptr<scalingsprite::ScalingSprite> scaling;

    std::shared_ptr<render::Texture2d> texture;
    window::Canvas                     canvas;
    core::vec2i                    mouse_popup = core::vec2i{0, 0};
    LineHoverData              hover;

    bool
    Run(const window::CanvasConfig& cc, const ScimedConfig& scc);
  };
}

#endif  // EUPHORIA_SCIMED_H
