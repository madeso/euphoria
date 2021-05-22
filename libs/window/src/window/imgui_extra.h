#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <functional>

#include "core/vec2.h"
#include "core/vec3.h"

// #include "imgui/imgui.h"

namespace euphoria::core
{
    struct angle;
    struct rgb;
    struct rgba;
    struct rgbai;
}

namespace euphoria::render
{
    struct texture2d;
}

struct ImVec2;
struct ImVec4;


namespace euphoria::window
{
    enum class corner
    {
        top_left = 0,
        top_right = 1,
        bottom_left = 2,
        bottom_right = 3,
        center = 4
    };

    ImVec2
    C(const core::vec2f &v);

    core::vec2f
    C(const ImVec2 &v);
}

namespace euphoria::window::imgui
{
    // knob based on https://github.com/ocornut/imgui/issues/942

    enum knob_style
    {
        // interaction style
        KS_UI_DRAG_X = 1 << 1,
        KS_UI_DRAG_Y = 1 << 2,
        KS_UI_AIM    = 1 << 3,

        // purely visible stuff
        KS_VIS_MARKERS_VISIBLE   = 1 << 5,
        KS_VIS_MAXMIN_VISIBLE    = 1 << 6,
        KS_VIS_OFF_MARKER_HIDDEN = 1 << 7,
        KS_VIS_DRAW_BACKGROUND   = 1 << 8,

        KS_VIS_VALUE_AS_TOOLTIP             = 1 << 9,
        KS_VIS_VALUE_INSTEAD_OF_CONTROLNAME = 1 << 10,
        KS_VIS_DISPLAY_VALUE_ON_HOVER       = 1 << 11,

        KS_DEFAULT =
              KS_UI_AIM
            | KS_VIS_MAXMIN_VISIBLE
            | KS_VIS_MARKERS_VISIBLE
            | KS_VIS_OFF_MARKER_HIDDEN | KS_VIS_DRAW_BACKGROUND
            | KS_VIS_DISPLAY_VALUE_ON_HOVER
            | KS_VIS_VALUE_INSTEAD_OF_CONTROLNAME
    };

    bool
    input_text(const char* label, std::string* str);

    void
    help_marker(const char* desc);

    void
    help_text(const char* desc);

    void
    label(const std::string& str);

    bool
    angle_slider
    (
        const char* const name,
        core::angle* angle,
        const core::angle& mindeg,
        const core::angle& maxdeg
    );

    bool
    toggle_button(const char* const label, bool down, const ImVec2& size);

    void
    image(render::texture2d* texture);

    bool
    color_edit(const char* const name, core::rgb* rgb);

    bool
    color_edit(const char* const name, core::rgba* rgb);

    bool
    color_edit(const char* const name, core::rgbai* rgb);

    bool
    begin_fixed_overlay
    (
        corner corner,
        const std::string& title,
        float distance = 10.0f,
        float distance_y = -1.0f
    );

    struct disabled
    {
        disabled();
        ~disabled();
    };

    bool
    selectable_or_disabled(bool enabled, const char* label);

    bool
    knob
    (
        const char* label,
        float*      p_value,
        float       v_min,
        float       v_max,
        knob_style   style = KS_DEFAULT
    );

    bool
    canvas_begin(const ImVec4& background_color, const char* title);

    void
    canvas_end();

    // wrappers over dear imgui functions to avoid exposting the whole dear imgui header
    bool
    begin_combo(const char* label, const char* preview);

    bool
    selectable(const char* label, bool is_selected);

    void
    end_combo();

    template<typename T>
    bool combo
    (
        const char* label,
        T* data,
        std::vector<std::pair<const char*, T>> values
    )
    {
        const auto found = std::find_if
        (
            values.begin(), values.end(),
            [data](const std::pair<const char*, T>& p)
            {
                return p.
                second == *data;
            }
        );

        const char* preview_value = found != values.end() ? found->first : "";

        bool was_changed = false;

        if(begin_combo(label, preview_value))
        {
            for(const auto& v: values)
            {
                if(selectable(v.first, v.second == *data))
                {
                    *data = v.second;
                    was_changed = true;
                }
            }
            end_combo();
        }

        return was_changed;
    }

    template <typename T, typename F>
    void
    custom_dropdown(const char* name, T* current, T max, F to_string)
    {
        if(begin_combo(name, to_string(*current).c_str()))
        {
            for(int i = 0; i < static_cast<int>(max); i += 1)
            {
                const auto o = static_cast<T>(i);
                if(selectable(to_string(o).c_str(), *current == o))
                {
                    *current = o;
                }
            }
            end_combo();
        }
    }
}
