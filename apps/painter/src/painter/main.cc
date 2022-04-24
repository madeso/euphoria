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

    void show_grid(const CanvasConfig& cc) const
    {
        canvas.show_grid(cc);
    }

    void show_ruler(const CanvasConfig& cc) const
    {
        canvas.show_ruler(cc);
    }

    void end(const CanvasConfig& cc)
    {
        canvas.end(cc);
    }

    std::pair<bool, Vec2f> handle(const ImVec2& p, int id, ImU32 hover_color, ImU32 default_color)
    {
        const auto radius = 6.0f;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const auto screen_position = canvas.world_to_screen(p);
        const auto mouse = ImGui::GetMousePos();
        const auto sq_dist_to_mouse = Vec2f::from_to(con(mouse), con(screen_position)).get_length_squared();
        const auto is_over = sq_dist_to_mouse < radius * radius;
        const auto is_hover = is_over || id == index;
        if(index == -1 && is_over && ImGui::IsMouseDown(0))
        {
            index = id;
        }
        draw_list->AddCircleFilled(screen_position, radius, is_hover ? hover_color : default_color);
        if(index == id)
        {
            // capture current drag item...
            if(ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                const auto d = ImGui::GetMouseDragDelta();
                ImGui::ResetMouseDragDelta();
                // todo(Gustav): handle scale/zoom
                return std::make_pair(true, Vec2f(d.x, d.y) / canvas.view.scale);
            }
        }
        return std::make_pair(false, Vec2f(0, 0));
    }

    void line(const Vec2f& from, const Vec2f& to, ImU32 line_color) const
    {
        ::line(canvas.world_to_screen(con(from)), canvas.world_to_screen(con(to)), line_color);
    }

    void bezier_cubic(const Vec2f& a0, const Vec2f& c0, const Vec2f& c1, const Vec2f& a1, ImU32 curve_color, float thickness = 1.0f) const
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


void
draw_link_lines(const CanvasWithControls& canvas, const PolyBezier2& path, ImU32 color)
{
    if(!path.is_autoset_enabled)
    {
        for(int seg: path.iterate_segments())
        {
            auto s = path.get_segment(seg);
            canvas.line(s.a0, s.c0, color);
            canvas.line(s.a1, s.c1, color);
        }
    }
}


struct PolyTheme
{
    static constexpr int default_shade = 9;
    static constexpr int handle_default_shade = 7;
    static constexpr int handle_hover_shade = 9;
    static constexpr auto anchor_point_color = oc_cyan;
    static constexpr auto control_point_color = oc_red;

    ImU32 link_line;
    ImU32 curve;
    ImU32 anchor_point_default;
    ImU32 anchor_point_hover;
    ImU32 control_point_default;
    ImU32 control_point_hover;

    constexpr PolyTheme()
        : link_line(oc_blue[default_shade])
        , curve(oc_gray[9])
        , anchor_point_default(anchor_point_color[handle_default_shade])
        , anchor_point_hover(anchor_point_color[handle_hover_shade])
        , control_point_default(control_point_color[handle_default_shade])
        , control_point_hover(control_point_color[handle_hover_shade])
    {
    }
};


void
handle_bezier_tweak(const PolyTheme& theme, CanvasWithControls& canvas, PolyBezier2& path)
{
    // draw link lines
    draw_link_lines(canvas, path, theme.link_line);

    // draw lines first to draw the handles abose the lines

    // draw handles
    for(auto point_index: path.iterate_points())
    {
        const bool is_anchor_point = PolyBezier2::is_anchor_point(point_index);
        if(path.is_autoset_enabled && !is_anchor_point)
        {
            continue;
        }

        auto r = canvas.handle
        (
            con(path.points[point_index]),
            euphoria::core::c_sizet_to_int(point_index),
            is_anchor_point ? theme.anchor_point_hover   : theme.control_point_hover,
            is_anchor_point ? theme.anchor_point_default : theme.control_point_default
        );
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
}


void
draw_bezier(const CanvasWithControls& canvas, const PolyBezier2& path, ImU32 color)
{
    // draw bezier and link lines
    for(int seg: path.iterate_segments())
    {
        auto s = path.get_segment(seg);
        canvas.bezier_cubic(s.a0, s.c0, s.c1, s.a1, color);
    }
}



void
handle_polybezier_rightclick(const CanvasWithControls& canvas, PolyBezier2& path)
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
            canvas.begin(cc);
            canvas.show_grid(cc);

            const auto theme = PolyTheme{};

            draw_bezier(canvas, path, theme.curve);
            handle_bezier_tweak(theme, canvas, path);

            canvas.show_ruler(cc);
            canvas.end(cc);

            handle_polybezier_rightclick(canvas, path);
        }
        ImGui::End();
    }
};


int
main(int argc, char** argv)
{
    return run_app(argc, argv, "Painter", []{ return std::make_unique<PainterApp>(); });
}
