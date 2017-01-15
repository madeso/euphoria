#ifndef GUI_SKIN_H
#define GUI_SKIN_H

#include "core/rgb.h"

#include <string>
#include <memory>

class Font;

class ButtonState {
 public:
  ButtonState();
  ~ButtonState();

  std::string image;
  float scale;
  Rgb image_color;
  Rgb text_color;
  float dx;
  float dy;
};

class Skin {
 public:
  Skin();
  ~Skin();

  std::string name;

  std::shared_ptr<Font> font;

  ButtonState button_idle;
  ButtonState button_hot;
  ButtonState button_active_hot;
};

#endif  // GUI_SKIN_H
