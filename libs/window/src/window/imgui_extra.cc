#include "window/imgui_extra.h"

#include "assert/assert.h"

#include "core/angle.h"
#include "core/rgb.h"
#include "core/numeric.h"

#include "render/texture.h"

#include "window/imgui_icons.h"


#include <iomanip>

#include "fmt/format.h"

#include "imgui/imgui_internal.h"
#include "imgui_stdlib.h"

#include "euph_generated_config.h"


namespace euphoria::window
{
    ImVec2
    con(const core::vec2f& v)
    {
        return ImVec2 {v.x, v.y};
    }


    core::vec2f
    con(const ImVec2& v)
    {
        return core::vec2f {v.x, v.y};
    }
}

namespace euphoria::window::imgui
{
    void
    help_marker(const char* desc)
    {
        ImGui::SameLine();
        ImGui::TextDisabled( ICON_MDI_HELP_CIRCLE );
        help_text(desc);
    }


    void
    help_text(const char* desc)
    {
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }


    void
    label(const std::string& str)
    {
        ImGui::TextUnformatted(str.c_str());
    }


    bool
    angle_slider
    (
        const char* name,
        core::angle* angle,
        const core::angle& mindeg,
        const core::angle& maxdeg
    )
    {
        ASSERT(angle);

        float degrees = angle->in_degrees();
        const auto value_was_changed = ImGui::SliderFloat
        (
            name,
            &degrees,
            mindeg.in_degrees(),
            maxdeg.in_degrees()
        );

        if(value_was_changed)
        {
            *angle = core::angle::from_degrees(degrees);
        }

        return value_was_changed;
    }


    bool
    angle_slider
    (
        const char* name,
        core::angle* angle
    )
    {
        ASSERT(angle);

        float degrees = angle->in_degrees();
        const auto value_was_changed = ImGui::DragFloat
        (
            name,
            &degrees
        );

        if(value_was_changed)
        {
            *angle = core::angle::from_degrees(degrees);
            angle->wrap();
        }

        return value_was_changed;
    }


    bool
    toggle_button(const char* label, bool down, const ImVec2& size)
    {
        if (down)
        {
            const auto c = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
            ImGui::PushStyleColor(ImGuiCol_Button, c);
            const auto r = ImGui::Button(label, size);
            ImGui::PopStyleColor(1);

            return r;
        }
        else
        {
            return ImGui::Button(label, size);
        }
    }

    bool
    color_edit(const char* name, core::rgb* c)
    {
        return ImGui::ColorEdit3(name, &c->r);
    }

    bool
    color_edit(const char* name, core::rgba* c)
    {
        return ImGui::ColorEdit4(name, &c->r);
    }

    bool
    color_edit(const char* name, core::rgbai* c)
    {
        auto cc = crgba(*c);
        const auto changed = ImGui::ColorEdit4(name, &cc.r);
        if(changed)
        {
            *c = crgbai(cc);
        }
        return changed;
    }


    ImTextureID
    c_texture_to_imgui(render::Texture2* texture)
    {
#if EUPH_ARCH_32 == 1
        std::int32_t
#elif EUPH_ARCH_64 == 1
        std::int64_t
#else
#error unknown arch
#endif
            id = texture->get_id();
        return reinterpret_cast<ImTextureID>(id);
    }


    void
    image(render::Texture2* texture)
    {
        auto tex_w = static_cast<float>(texture->width);
        auto tex_h = static_cast<float>(texture->height);
        ImTextureID tex_id = c_texture_to_imgui(texture); // NOLINT: auto is preferred but a texture is a hidden pointer

        ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
        label(fmt::format("{:.0}x{:.0}", tex_w, tex_h));
        ImGui::Image(
                tex_id,
                ImVec2(tex_w, tex_h),
                ImVec2(0, 0),
                ImVec2(1, 1),
                ImColor(255, 255, 255, 255),
                ImColor(255, 255, 255, 128));
        if(ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            float focus_sz = 32.0f;
            float focus_x = ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f;
            if(focus_x < 0.0f)
            {
                focus_x = 0.0f;
            }
            else if(focus_x > tex_w - focus_sz)
            {
                focus_x = tex_w - focus_sz;
            }
            float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y
                            - focus_sz * 0.5f;
            if(focus_y < 0.0f)
            {
                focus_y = 0.0f;
            }
            else if(focus_y > tex_h - focus_sz)
            {
                focus_y = tex_h - focus_sz;
            }
            label(fmt::format("Min: ({:.2f}, {:.2f})", focus_x, focus_y));
            label(fmt::format("Max: ({:.2f}, {:.2f})", focus_x + focus_sz, focus_y + focus_sz));
            ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
            ImVec2 uv1 = ImVec2(
                    (focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
            ImGui::Image(
                    tex_id,
                    ImVec2(128, 128),
                    uv0,
                    uv1,
                    ImColor(255, 255, 255, 255),
                    ImColor(255, 255, 255, 128));
            ImGui::EndTooltip();
        }
    }


    // stolen from ShowExampleAppFixedOverlay function in imgui_demo
    bool
    begin_fixed_overlay
    (
            Corner corner,
            const std::string& title,
            float a_distance,
            float a_distance_y
    )
    {
        const int corner_int = static_cast<int>(corner);
        // const float distance = 10.0f;
        const auto distance_x = a_distance;
        const auto distance_y = a_distance_y > 0 ? a_distance_y : a_distance;
        const auto size = ImGui::GetIO().DisplaySize;
        const ImVec2 window_pos = corner == Corner::center
            ? ImVec2(size.x / 2, size.y / 2)
            : ImVec2
            (
                (corner_int & 1) != 0
                    ? size.x - distance_x
                    : distance_x,
                (corner_int & 2) != 0
                    ? size.y - distance_y
                    : distance_y
            );
        const ImVec2 window_pos_pivot = corner == Corner::center
            ? ImVec2(0.5f, 0.5f)
            : ImVec2
            (
                (corner_int & 1) != 0 ? 1.0f : 0.0f,
                (corner_int & 2) != 0 ? 1.0f : 0.0f
            );
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

        return ImGui::Begin(
                title.c_str(),
                nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
                        | ImGuiWindowFlags_AlwaysAutoResize
                        | ImGuiWindowFlags_NoMove
                        | ImGuiWindowFlags_NoSavedSettings);
    }


    // from https://github.com/ocornut/imgui/issues/211
    VisuallyDisabledWidgets::VisuallyDisabledWidgets()
    {
        // todo(Gustav): change to use BeginDisabled / EndDisabled instead?
        // ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(
                ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }


    VisuallyDisabledWidgets::~VisuallyDisabledWidgets()
    {
        // ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }


    bool
    selectable_or_disabled(bool enabled, const char* label)
    {
        if(enabled)
        {
            if(ImGui::Selectable(label))
            {
                return true;
            }
        }
        else
        {
            VisuallyDisabledWidgets disabled;
            ImGui::TextUnformatted(label);
        }

        return false;
    }



    void
    add_circle_filled
    (
        ImDrawList* draw_list,
        const ImVec2& centre,
        float radius,
        ImU32 col,
        int num_segments,
        float angle_offset
    )
    {
        ASSERT(draw_list);

        // implementation modified from ImDrawList AddCircleFilled

        if((col & IM_COL32_A_MASK) == 0)
        { return; }

        const float a_max = IM_PI * 2.0f * (core::c_int_to_float(num_segments) - 1.0f) / core::c_int_to_float(num_segments);
        draw_list->PathArcTo
        (
            centre,
            radius,
            angle_offset,
            a_max + angle_offset,
            num_segments
        );
        draw_list->PathFillConvex(col);
    }

    bool
    knob
    (
        const char* label,
        float* p_value,
        float v_min,
        float v_max,
        KnobStyle style
    )
    {
        constexpr auto pi = core::pi;
        // constexpr auto rad2deg = 180 / pi;
        constexpr auto pi2 = pi * 2;
        constexpr float angle_min = pi * 0.75f;
        constexpr float angle_max = pi * 2.25f;
        constexpr float angle_step = 20 * (pi / 180);
        constexpr float size_outer = 20;
        constexpr float peg_max_end = size_outer;
        constexpr float peg_end = 19;
        constexpr float peg_start = 15;
        constexpr float knob_size = 15;
        constexpr float knob_mark_start = 15;
        constexpr float knob_mark_end = 8;
        constexpr int seg = 16;

        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& imstyle = ImGui::GetStyle();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const ImVec2 start = ImGui::GetCursorScreenPos();
        const ImVec2 center
                = ImVec2(start.x + size_outer, start.y + size_outer);
        const float line_height = ImGui::GetTextLineHeight();

        ImGui::InvisibleButton
        (
            label,
            ImVec2
            (
                size_outer * 2,
                size_outer * 2 + line_height + imstyle.ItemInnerSpacing.y
            )
       );
        const bool is_active = ImGui::IsItemActive();
        const bool is_hovered = ImGui::IsItemHovered();

        const float t = (*p_value - v_min) / (v_max - v_min);
        const float angle = angle_min + (angle_max - angle_min) * t;

        // changing value
        bool value_changed = false;

        if((style & knob_style_ui_aim) != 0)
        {
            ImVec2 direct(io.MousePos.x - center.x, io.MousePos.y - center.y);
            const float directl = sqrtf(direct.x * direct.x + direct.y * direct.y);
            direct.x = direct.x / directl;
            direct.y = direct.y / directl;

            // todo(Gustav): flip y based on x
            const auto acos = acosf(direct.x);
            const auto ang = direct.y > 0 ? pi2 - acos : acos;
            float input_angle = -ang;
            input_angle += pi2;
            const bool b = input_angle < pi / 2;
            if(b)
            {
                input_angle += pi2;
            }

            const float input_angle_t
                    = (input_angle - angle_min) / (angle_max - angle_min);

            if(is_active && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
            {
                *p_value = v_min + (v_max - v_min) * input_angle_t;

                if(*p_value < v_min) { *p_value = v_min; }
                if(*p_value > v_max) { *p_value = v_max; }
            }
        }
        else
        {
            const auto val = (style & knob_style_ui_drag_x) != 0 ? io.MouseDelta.x : io.MouseDelta.y;
            if(is_active && val != 0.0f)
            {
                float step = (v_max - v_min) / 200.0f;
                *p_value += val * step;
                if(*p_value < v_min) { *p_value = v_min; }
                if(*p_value > v_max) { *p_value = v_max; }

                value_changed = true;
            }
        }

        // colors
        const auto label_color = ImGui::GetColorU32(ImGuiCol_Text);
        const auto fill_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
        const auto knob_color = ImGui::GetColorU32(ImGuiCol_SliderGrab);
        const auto indicator_color = ImGui::GetColorU32(ImGuiCol_Text);
        const auto peg_color_off = ImGui::GetColorU32(ImGuiCol_TextDisabled);
        const auto peg_color_on = ImGui::GetColorU32(ImGuiCol_Text);
        const auto peg_color_max = ImGui::GetColorU32(ImGuiCol_Text);

        // util function
        const auto calculate_position = [=](float ang, float rad) -> ImVec2
        {
            return ImVec2(center.x + cosf(ang) * rad, center.y + sinf(ang) * rad);
        };

        // ----------------- visualization
        // background
        if((style & knob_style_vis_draw_background) != 0)
        {
            draw_list->AddCircleFilled(center, size_outer, fill_color, seg);
        }

        // peg indicators
        if((style & knob_style_vis_markers_visible) != 0)
        {
            const auto marker_stop = ((style & knob_style_vis_off_marker_hidden) != 0) ? angle : angle_max;
            for(float a = angle_min; a <= marker_stop; a += angle_step)
            {
                const auto c = (style & knob_style_vis_off_marker_hidden) != 0
                      ? peg_color_off
                      : a <= angle ? peg_color_on : peg_color_off
                      ;
                draw_list->AddLine(calculate_position(a, peg_start), calculate_position(a, peg_end), c, 1.0f);
            }
        }

        if((style & knob_style_vis_max_and_min_visible) != 0)
        {
            draw_list->AddLine(calculate_position(angle_max, peg_start), calculate_position(angle_max, peg_max_end), peg_color_max, 1.0f);
            draw_list->AddLine(calculate_position(angle_min, peg_start), calculate_position(angle_min, peg_max_end), peg_color_max, 1.0f);
        }

        // the knob
        add_circle_filled(draw_list, center, knob_size, knob_color, 6, angle);
        draw_list->AddLine(calculate_position(angle, knob_mark_start), calculate_position(angle, knob_mark_end), indicator_color, 2.0f);

        const bool display_value = (style & knob_style_vis_display_value_on_hover) != 0
            ? is_active || is_hovered
            : is_active
            ;

        // knob control name
        const auto label_position = ImVec2 {start.x, start.y + size_outer * 2 + imstyle.ItemInnerSpacing.y / 4};

        const auto value_to_str = [](float f) -> std::string
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3) << f;
            return ss.str();
        };

        if((style & knob_style_vis_value_instead_of_name) != 0 && display_value)
        {
            const auto v = value_to_str(*p_value);
            draw_list->AddText(label_position, label_color, v.c_str());
        }
        else
        {
            draw_list->AddText(label_position, label_color, label);
        }

        // tooltip
        if((style & knob_style_vis_value_as_tooltip) != 0 && display_value)
        {
            ImGui::SetNextWindowPos
            (
                ImVec2
                (
                    start.x - 0,
                    start.y - line_height - imstyle.ItemInnerSpacing.y - imstyle.WindowPadding.y
                )
            );
            ImGui::BeginTooltip();
            const auto v = value_to_str(*p_value);
            ImGui::Text("%s", v.c_str());
            ImGui::EndTooltip();
        }

        return value_changed;
    }

    bool
    canvas_begin(const ImVec4& background_color, const char* title)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, background_color);

        return ImGui::BeginChild
        (
            title,
            ImVec2(0, 0),
            true,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove
        );
    }

    void
    canvas_end()
    {
        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    bool
    begin_combo(const char* label, const char* preview)
    {
        return ImGui::BeginCombo(label, preview);
    }

    bool
    selectable(const char* label, bool is_selected)
    {
        return ImGui::Selectable(label, is_selected);
    }

    void
    end_combo()
    {
        return ImGui::EndCombo();
    }
}
