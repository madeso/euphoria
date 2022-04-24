#include "core/polybezier.h"

#include "window/canvas.h"
#include "window/open_color.imgui.h"
#include "window/app.h"

#include "window/imgui_extra.h"


using namespace euphoria::core;
using namespace euphoria::window;


void line(const ImVec2& from, const ImVec2& to, ImU32 color)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->PathLineTo(from);
    draw_list->PathLineTo(to);
    draw_list->PathStroke(color, ImDrawFlags_None);
}


struct CanvasWithControls
{
    Canvas canvas;
    int index = -1;

    void begin(const CanvasConfig& cc)
    {
        canvas.begin(cc);

        if(ImGui::IsMouseReleased(0))
        {
            index = -1;
        }
    }

    void show_grid(const CanvasConfig& cc)
    {
        canvas.show_grid(cc);
    }

    void show_ruler(const CanvasConfig& cc)
    {
        canvas.show_ruler(cc);
    }

    void end(const CanvasConfig& cc)
    {
        canvas.end(cc);
    }

    std::pair<bool, Vec2f> handle(const ImVec2& p, int id, ImU32 hover_color, ImU32 default_color)
    {
        const auto size = 6.0f;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const auto sp = canvas.world_to_screen(p);
        const auto me = ImGui::GetMousePos();
        const auto hover
                = Vec2f::from_to(con(me), con(sp)).get_length_squared()
                    < size * size;
        if(index == -1 && hover && ImGui::IsMouseDown(0))
        {
            index = id;
        }
        draw_list->AddCircleFilled(sp, size, hover ? hover_color : default_color);
        if(index == id)
        {
            // capture current drag item...
            if(ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                const auto d = ImGui::GetMouseDragDelta();
                ImGui::ResetMouseDragDelta();
                // todo(Gustav): handle scale/zoom
                return std::make_pair(
                        true, Vec2f(d.x, d.y) / canvas.view.scale);
            }
        }
        return std::make_pair(false, Vec2f(0, 0));
    }

    void line(const Vec2f& from, const Vec2f& to, ImU32 line_color)
    {
        ::line(canvas.world_to_screen(con(from)), canvas.world_to_screen(con(to)), line_color);
    }

    void bezier_cubic(const Vec2f& a0, const Vec2f& c0, const Vec2f& c1, const Vec2f& a1, ImU32 curve_color, float thickness = 1.0f)
    {
        auto* dl = ImGui::GetWindowDrawList();
        dl->AddBezierCubic
        (
            canvas.world_to_screen(con(a0)),
            canvas.world_to_screen(con(c0)),
            canvas.world_to_screen(con(c1)),
            canvas.world_to_screen(con(a1)),
            curve_color,
            thickness
        );
    }
};




struct PainterApp : App
{
    CanvasConfig cc;
    CanvasWithControls canvas;
    PolyBezier2 path = Vec2f{0, 0};

    void on_gui() override
    {
        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Exit", "Ctrl+Q"))
                {
                    running = false;
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

        ImGui::SetNextWindowSize(ImVec2 {300, 300}, ImGuiCond_FirstUseEver);
        if(ImGui::Begin("painter"))
        {
            // win->Run(&style_data);
            const auto a = !ImGui::IsAnyItemHovered();
            // const auto b = ImGui::IsMouseHoveringWindow();
            const auto c = ImGui::IsMouseClicked(1);
            // LOG_INFO("bools " << a << " " << b << " " << c);
            if(a && c)
            {
                ImGui::OpenPopup("context_menu");
            }

            canvas.begin(cc);
            canvas.show_grid(cc);

            constexpr auto default_shade = 9;
            constexpr auto handle_default_shade = 7;
            constexpr auto handle_hover_shade = 9;
            constexpr auto curve_color = oc_gray[9]; // IM_COL32(0, 0, 200, 255);
            constexpr auto line_color = oc_blue[default_shade]; //IM_COL32(0, 0, 0, 255);
            constexpr auto anchor_point_color = oc_cyan;
            constexpr auto control_point_color = oc_red;

            // draw handles
            for(auto point_index: path.iterate_points())
            {
                const bool is_anchor_point = PolyBezier2::is_anchor_point(point_index);
                if(path.is_autoset_enabled && !is_anchor_point)
                {
                    continue;
                }
                const auto& color = is_anchor_point ? anchor_point_color : control_point_color;
                auto r = canvas.handle(con(path.points[point_index]), euphoria::core::c_sizet_to_int(point_index), color[handle_hover_shade], color[handle_default_shade]);
                if(r.first)
                {
                    if(ImGui::GetIO().KeyCtrl)
                    {
                        path.points[point_index] += r.second;
                    }
                    else
                    {
                        path.move_point(euphoria::core::c_sizet_to_int(point_index), r.second);
                    }
                }
            }

            // draw bezier and link lines
            for(int seg: path.iterate_segments())
            {
                auto s = path.get_segment(seg);
                canvas.bezier_cubic(s.a0, s.c0, s.c1, s.a1, curve_color);

                if(!path.is_autoset_enabled)
                {
                    canvas.line(s.a0, s.c0, line_color);
                    canvas.line(s.a1, s.c1, line_color);
                }
            }

            canvas.show_ruler(cc);
            canvas.end(cc);

            if(ImGui::BeginPopup("context_menu"))
            {
                const auto p = canvas.canvas.screen_to_world(ImGui::GetMousePosOnOpeningCurrentPopup());
                if(ImGui::MenuItem("Add"))
                {
                    path.add_point(con(p));
                }
                auto ic = path.is_closed;
                if(ImGui::Checkbox("Is closed", &ic))
                {
                    path.toggle_closed();
                }
                auto as = path.is_autoset_enabled;
                if(ImGui::Checkbox("Autoset control points", &as))
                {
                    path.toggle_auto_set_control_points();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }
};


int
main(int argc, char** argv)
{
    return run_app(argc, argv, "Painter", []{ return std::make_unique<PainterApp>(); });
}
