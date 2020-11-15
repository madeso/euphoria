#ifndef EUPHORIA_WINDOW_TIMGUI_H
#define EUPHORIA_WINDOW_TIMGUI_H

#include <string>
#include <functional>

#include "imgui/imgui.h"

namespace euphoria::window
{
    template <typename T, typename F>
    void
    CustomDropdown(const char* name, T* current, T max, F to_string)
    {
        if(ImGui::BeginCombo(name, to_string(*current).c_str()))
        {
            for(int i = 0; i < static_cast<int>(max); i += 1)
            {
                const auto o = static_cast<T>(i);
                if(ImGui::Selectable(to_string(o).c_str(), *current == o))
                {
                    *current = o;
                }
            }
            ImGui::EndCombo();
        }
    }
}  // namespace euphoria::window

#endif  // EUPHORIA_WINDOW_TIMGUI_H