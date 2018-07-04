#ifndef EUPHORIA_VIEWPORT_H
#define EUPHORIA_VIEWPORT_H

#include "core/rect.h"

struct ViewportDef
{
  Recti screen_rect;

  float virtual_width;
  float virtual_height;

  /// Fits the viewport, scaling it, keeping the aspect ratio.
  /// Black bars may appear if the aspect ration doesnt match
  static ViewportDef
  FitWithBlackBars(
      float width, float height, int window_width, int window_height);

  /// Fits the viewport, scaling it to the max and then fits the viewport
  /// without stretching. This means that the viewport isnt the same size as
  /// requested.
  static ViewportDef
  Extend(float width, float height, int window_width, int window_height);

  /// The viewport matches the screen pixel by pixel.
  static ViewportDef
  ScreenPixel(int window_width, int window_height);

  ViewportDef(const Recti& screen, float w, float h);
};


#endif  // EUPHORIA_VIEWPORT_H
