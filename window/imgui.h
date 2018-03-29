#ifndef EUPHORIA_IMGUI_H
#define EUPHORIA_IMGUI_H

#include <string>

class Angle;
class Texture2d;
class Rgb;

void
ImguiAngleSlider(
    const char* const name, Angle* angle, float mindeg, float maxdeg);

void
ImguiImage(Texture2d* texture);

void
ImGuiColorEdit3(const char* const name, Rgb* rgb);

enum class ImguiCorner
{
  TopLeft     = 0,
  TopRight    = 1,
  BottomLeft  = 2,
  BottomRight = 3,
  Center      = 4
};

bool
BeginFixedOverlay(ImguiCorner corner, const std::string& title);

#endif  // EUPHORIA_IMGUI_H
