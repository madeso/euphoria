#pragma once

#include "core/canvaslogic.h"

#include "imgui/imgui.h"

namespace euphoria::window
{
    struct canvas_config
    {
        float zoom_speed = 10;
        float grid_size = 64.0f;
        ImU32 grid_color = IM_COL32(200, 200, 200, 40);
        ImU32 background_color = IM_COL32(60, 60, 70, 200);
    };

    struct canvas
    {
        core::canvas_logic view;
        ImVec2 position = ImVec2 {0, 0};

        void
        show_grid(const canvas_config& cc) const;

        void
        vertical_line(float rx, ImU32 grid_color) const;

        void
        horizontal_line(float ry, ImU32 grid_color) const;

        void
        show_ruler(float ruler_interval, ImU32 ruler_color, float length) const;

        void
        show_ruler(const canvas_config& cc) const;

        void
        begin(const canvas_config& cc);

        void
        end(const canvas_config& cc);

        [[nodiscard]] ImVec2
        world_to_screen(const ImVec2& v) const;

        [[nodiscard]] ImVec2
        world_to_screen_size(const ImVec2& v) const;

        [[nodiscard]] ImVec2
        screen_to_world(const ImVec2& v) const;

        [[nodiscard]] ImVec2
        get_mouse() const;
    };
}
