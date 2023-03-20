#pragma once

#include <algorithm>
#include <utility>
#include <functional>

#include "core/vec2.h"
#include "core/vec3.h"

#include "imgui/imgui.h"


namespace euphoria::core
{
    struct angle;
    struct rgb;
    struct rgba;
    struct rgbai;
}

namespace euphoria::render
{
    struct Texture2;
}

struct ImVec2;
struct ImVec4;


namespace euphoria::window
{
    enum class Corner
    {
        top_left = 0,
        top_right = 1,
        bottom_left = 2,
        bottom_right = 3,
        center = 4
    };

    ImVec2
    con(const core::vec2f &v);

    core::vec2f
    con(const ImVec2 &v);
}

namespace euphoria::window::imgui
{
    // knob based on https://github.com/ocornut/imgui/issues/942

    enum KnobStyle
    {
        // interaction style
        knob_style_ui_drag_x = 1 << 1,
        knob_style_ui_drag_y = 1 << 2,
        knob_style_ui_aim = 1 << 3,

        // purely visible stuff
        knob_style_vis_markers_visible = 1 << 5,
        knob_style_vis_max_and_min_visible = 1 << 6,
        knob_style_vis_off_marker_hidden = 1 << 7,
        knob_style_vis_draw_background = 1 << 8,

        knob_style_vis_value_as_tooltip = 1 << 9,
        knob_style_vis_value_instead_of_name = 1 << 10,
        knob_style_vis_display_value_on_hover = 1 << 11,

        knob_style_default =
              knob_style_ui_aim
            | knob_style_vis_max_and_min_visible
            | knob_style_vis_markers_visible
            | knob_style_vis_off_marker_hidden
            | knob_style_vis_draw_background
            | knob_style_vis_display_value_on_hover
            | knob_style_vis_value_instead_of_name
    };

    void
    add_help_marker_for_previous_widget(const char* desc);

    void
    add_help_text_for_previous_widget(const char* desc);

    void
    label(const std::string& str);

    bool
    imgui_angle_slider
    (
        const char* name,
        core::angle* angle,
        const core::angle& min_deg,
        const core::angle& max_deg
    );

    bool
    imgui_angle_slider
    (
        const char* name,
        core::angle* angle
    );

    bool
    imgui_toggle_button(const char* label, bool down, const ImVec2& size);


    ImTextureID
    c_texture_to_imgui(render::Texture2* texture);


    void
    image(render::Texture2* texture);

    bool
    imgui_color_edit(const char* name, core::rgb* rgb);

    bool
    imgui_color_edit(const char* name, core::rgba* rgb);

    bool
    imgui_color_edit(const char* name, core::rgbai* rgb);

    bool
    begin_fixed_overlay
    (
        Corner corner,
        const std::string& title,
        float distance = 10.0f,
        float distance_y = -1.0f
    );

    struct VisuallyDisabledWidgets
    {
        VisuallyDisabledWidgets();
        ~VisuallyDisabledWidgets();

        VisuallyDisabledWidgets(const VisuallyDisabledWidgets& other) = delete;
        void operator=(const VisuallyDisabledWidgets&) = delete;
        VisuallyDisabledWidgets(VisuallyDisabledWidgets&& other) = delete;
        void operator=(VisuallyDisabledWidgets&&) = delete;
    };

    bool
    imgui_selectable_or_disabled(bool enabled, const char* label);

    bool
    knob
    (
        const char* label,
        float* p_value,
        float v_min,
        float v_max,
        KnobStyle style = knob_style_default
    );

    bool
    begin_canvas_widget(const ImVec4& background_color, const char* title);

    void
    end_canvas_widget();

    // wrappers over dear imgui functions to avoid exposing the whole dear imgui header
    bool
    begin_combo(const char* label, const char* preview);

    bool
    imgui_selectable(const char* label, bool is_selected);

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
                if(imgui_selectable(v.first, v.second == *data))
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
    imgui_custom_dropdown(const char* name, T* current, T max, F to_string)
    {
        if(begin_combo(name, to_string(*current).c_str()))
        {
            for(int i = 0; i < static_cast<int>(max); i += 1)
            {
                const auto o = static_cast<T>(i);
                if(imgui_selectable(to_string(o).c_str(), *current == o))
                {
                    *current = o;
                }
            }
            end_combo();
        }
    }
}
