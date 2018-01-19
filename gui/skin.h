#ifndef GUI_SKIN_H
#define GUI_SKIN_H

#include <string>
#include <memory>

#include "core/rgb.h"
#include "core/interpolate.h"

class Font;

class ButtonState
{
 public:
  ButtonState();
  ~ButtonState();

  // std::string image;
  float scale;
  Rgb   image_color;
  Rgb   text_color;
  float dx;
  float dy;

  InterpolationType interpolationPosition;
  float             interpolationPositionTime;

  InterpolationType interpolationColor;
  float             interpolationColorTime;

  InterpolationType interpolationSize;
  float             interpolationSizeTime;
};

class Skin
{
 public:
  Skin();
  ~Skin();

  std::string name;

  std::shared_ptr<Font> font;

  float text_size;

  std::string button_image;
  ButtonState button_idle;
  ButtonState button_hot;
  ButtonState button_active_hot;
};

#endif  // GUI_SKIN_H
