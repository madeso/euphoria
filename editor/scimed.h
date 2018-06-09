#ifndef EUPHORIA_SCIMED_H
#define EUPHORIA_SCIMED_H

#include <memory>

#include "core/vec2.h"

#include "editor/canvas.h"

#include "gaf_scalingsprite.h"

class Texture2d;
class TextureCache;
class FileSystem;

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
  scalingsprite::ScalingSprite scaling;

  std::shared_ptr<Texture2d> texture;
  Canvas                     canvas;
  vec2i                      mouse_popup = vec2i{0, 0};
  LineHoverData              hover;

  std::string name;

  void
  LoadFile(TextureCache* cache, FileSystem* fs, const std::string& path);

  bool
  Run(const CanvasConfig& cc, const ScimedConfig& scc);
};


#endif  // EUPHORIA_SCIMED_H
