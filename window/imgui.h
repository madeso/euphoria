#ifndef EUPHORIA_IMGUI_H
#define EUPHORIA_IMGUI_H

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

#endif  // EUPHORIA_IMGUI_H
