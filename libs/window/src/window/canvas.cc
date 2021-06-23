#include "window/canvas.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

#include "window/imgui_extra.h"

using namespace euphoria::core;
using euphoria::window::con;

namespace euphoria::window
{
    void
    canvas::show_grid(const canvas_config& cc) const
    {
        const auto size = ImGui::GetWindowSize();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const float scaled_grid_size = cc.grid_size * view.scale;

        for(float x = fmodf(view.scroll.x, scaled_grid_size); x < size.x; x += scaled_grid_size)
        {
            draw_list->AddLine
            (
                ImVec2(x, 0.0f) + position,
                ImVec2(x, size.y) + position,
                cc.grid_color
            );
        }

        for(float y = fmodf(view.scroll.y, scaled_grid_size); y < size.y; y += scaled_grid_size)
        {
            draw_list->AddLine
            (
                ImVec2(0.0f, y) + position,
                ImVec2(size.x, y) + position,
                cc.grid_color
            );
        }
    }

    void
    canvas::vertical_line(float rx, ImU32 grid_color) const
    {
        const auto x = view.scroll.x + rx * view.scale;
        const auto size = ImGui::GetWindowSize();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddLine
        (
            ImVec2(x, 0.0f) + position,
            ImVec2(x, size.y) + position,
            grid_color
        );
    }

    void
    canvas::horizontal_line(float ry, ImU32 grid_color) const
    {
        const auto y = view.scroll.y + ry * view.scale;
        const auto size = ImGui::GetWindowSize();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddLine
        (
            ImVec2(0.0f, y) + position,
            ImVec2(size.x, y) + position,
            grid_color
        );
    }

    void
    canvas::show_ruler(float ruler_interval, ImU32 ruler_color, float length) const
    {
        const auto size = ImGui::GetWindowSize();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const float scaled_grid_size = ruler_interval * view.scale;

        for(float x = fmodf(view.scroll.x, scaled_grid_size); x < size.x;
            x += scaled_grid_size)
        {
            draw_list->AddLine
            (
                ImVec2(x, 0.0f) + position,
                ImVec2(x, length) + position,
                ruler_color
            );
        }

        for(float y = fmodf(view.scroll.y, scaled_grid_size); y < size.y;
            y += scaled_grid_size)
        {
            draw_list->AddLine
            (
                ImVec2(0.0f, y) + position,
                ImVec2(length, y) + position,
                ruler_color
            );
        }
    }

    void
    canvas::show_ruler(const canvas_config& cc) const
    {
        show_ruler(5.0f, cc.grid_color, 10.0f);
    }

    void
    canvas::begin(const canvas_config& cc)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, cc.background_color);

        ImGui::BeginChild
        (
            "scrolling_region",
            ImVec2(0, 0),
            true,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove
        );

        position = ImGui::GetCursorScreenPos();
    }

    void
    do_canvas_scroll(canvas* canvas)
    {
        if(ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        {
            canvas->view.pan(con(ImGui::GetIO().MouseDelta));
        }
    }

    void
    do_canvas_zoom(canvas* canvas, const canvas_config& cc)
    {
        if(ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive())
        {
            const auto mouse = ImGui::GetIO().MousePos - canvas->position;
            const auto zoom = ImGui::GetIO().MouseWheel;
            canvas->view.zoom(con(mouse), zoom * cc.zoom_speed);
        }
    }

    void
    canvas::end(const canvas_config& cc)
    {
        do_canvas_scroll(this);
        do_canvas_zoom(this, cc);

        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    ImVec2
    canvas::world_to_screen(const ImVec2& v) const
    {
        return con(view.world_to_screen(con(v))) + position;
    }

    ImVec2
    canvas::world_to_screen_size(const ImVec2& v) const
    {
        return v * view.scale;
    }

    ImVec2
    canvas::screen_to_world(const ImVec2& v) const
    {
        return con(view.screen_to_world(con(v - position)));
    }

    ImVec2
    canvas::get_mouse() const
    {
        return ImGui::GetMousePos() - position;
    }
}
