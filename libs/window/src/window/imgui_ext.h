#ifndef EUPHORIA_IMGUI_H
#define EUPHORIA_IMGUI_H

#include <string>

#include "core/vec2.h"
#include "core/vec3.h"

// imgui combo
#include "imgui/imgui.h"
#include <vector>
#include <algorithm>
#include <utility>

namespace euphoria::core
{
    struct Angle;
    struct rgb;
    struct rgba;
    struct rgbai;
}

namespace euphoria::render
{
    struct texture2d;
}

struct ImVec2;


namespace euphoria::window
{
    bool
    InputText(const char* label, std::string* str);

    void
    HelpMarker(const char* desc);

    void
    HelpText(const char* desc);

    void
    ImguiLabel(const std::string& str);

    bool
    ImguiAngleSlider
    (
        const char* const name,
        core::Angle* angle,
        const core::Angle& mindeg,
        const core::Angle& maxdeg
    );

    bool
    ToggleButton(const char* const label, bool down, const ImVec2& size);

    void
    ImguiImage(render::texture2d* texture);

    bool
    ImGuiColorEdit(const char* const name, core::rgb* rgb);

    bool
    ImGuiColorEdit(const char* const name, core::rgba* rgb);

    bool
    ImGuiColorEdit(const char* const name, core::rgbai* rgb);

    enum class ImguiCorner
    {
        TopLeft     = 0,
        TopRight    = 1,
        BottomLeft  = 2,
        BottomRight = 3,
        Center      = 4
    };

    bool
    BeginFixedOverlay
    (
        ImguiCorner corner,
        const std::string& title,
        float distance = 10.0f,
        float distance_y = -1.0f
    );

    ImVec2
    C(const core::vec2f& v);

    core::vec2f
    C(const ImVec2& v);

    struct ImguiDisabled
    {
        ImguiDisabled();
        ~ImguiDisabled();
    };

    bool
    ImguiSelectableOrDisabled(bool enabled, const char* label);


    template<typename T>
    bool ImguiCombo
    (
        const char* label,
        T* data,
        std::vector<std::pair<const char*, T>> values
    )
    {
        const auto found = std::find_if(values.begin(), values.end(),
            [data](const std::pair<const char*, T>& p)
            {
                return p.second == *data;
            }
        );

        const char* preview_value = found != values.end() ? found->first : "";

        bool was_changed = false;

        if(ImGui::BeginCombo(label, preview_value))
        {
            for(const auto& v: values)
            {
                if(ImGui::Selectable(v.first, v.second == *data))
                {
                    *data = v.second;
                    was_changed = true;
                }
            }
            ImGui::EndCombo();
        }

        return was_changed;
    }
}

#endif  // EUPHORIA_IMGUI_H
