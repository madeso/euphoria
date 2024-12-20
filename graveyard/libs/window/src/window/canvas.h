#pragma once

#include "core/canvaslogic.h"

#include "imgui/imgui.h"

namespace eu::window
{
    struct CanvasConfig
    {
        float zoom_speed = 10;
        float grid_size = 64.0f;
        ImU32 grid_color = IM_COL32(200, 200, 200, 40);
        ImU32 background_color = IM_COL32(60, 60, 70, 200);
    };

    struct Canvas
    {
        core::CanvasLogic view;
        ImVec2 position = ImVec2 {0, 0};

        void begin(const CanvasConfig& cc);
        void end(const CanvasConfig& cc);

        void show_grid(const CanvasConfig& cc) const;
        void draw_vertical_line(float rx, ImU32 grid_color) const;
        void draw_horizontal_line(float ry, ImU32 grid_color) const;
        void show_ruler(float ruler_interval, ImU32 ruler_color, float length) const;
        void show_ruler(const CanvasConfig& cc) const;

        [[nodiscard]] ImVec2 from_world_to_screen(const ImVec2& v) const;
        [[nodiscard]] ImVec2 from_world_to_screen_size(const ImVec2& v) const;
        [[nodiscard]] ImVec2 from_screen_to_world(const ImVec2& v) const;

        [[nodiscard]] ImVec2 get_mouse() const;
    };
}
