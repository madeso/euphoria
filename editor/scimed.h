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

struct Scimed
{
  scalingsprite::ScalingSprite scaling;

  std::shared_ptr<Texture2d> texture;

  Canvas        canvas;
  vec2f         mouse_popup = vec2f{0, 0};
  LineHoverData hover;

  void
  LoadFile(TextureCache* cache, FileSystem* fs, const std::string& path);

  bool
  Run();
};


#endif  // EUPHORIA_SCIMED_H
