#ifndef EUPHORIA_WINDOW_SOLARIZED_H
#define EUPHORIA_WINDOW_SOLARIZED_H

struct ImGuiStyle;

namespace euphoria::window
{
    void
    SetupSolarized(bool light, ImGuiStyle* dst = nullptr);
}

#endif  // EUPHORIA_WINDOW_SOLARIZED_H
