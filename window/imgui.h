#ifndef EUPHORIA_IMGUI_H
#define EUPHORIA_IMGUI_H

class Angle;
class Texture2d;
void
ImguiAngleSlider(
    const char* const name, Angle* angle, float mindeg, float maxdeg);


void
ImguiImage(Texture2d* texture);

#endif  // EUPHORIA_IMGUI_H
