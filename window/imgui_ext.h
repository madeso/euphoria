#ifndef EUPHORIA_IMGUI_H
#define EUPHORIA_IMGUI_H

#include <string>
#include "core/vec2.h"

class Angle;
class Texture2d;
class Rgb;

struct ImVec2;

bool
InputText(const char* label, std::string* str);

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

ImVec2
C(const vec2f& v);

vec2f
C(const ImVec2& v);

#endif  // EUPHORIA_IMGUI_H
