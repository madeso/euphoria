#include "core/mat4.h"
#include "core/random.h"
#include "core/mat4.h"
#include "core/axisangle.h"
#include "core/aabb.h"
#include "core/texturetypes.h"
#include "core/vfs.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_path.h"
#include "core/os.h"
#include "core/range.h"
#include "core/camera3.h"
#include "core/stringutils.h"
#include "core/stdutils.h"
#include "core/proto.h"
#include "core/log.h"
#include "core/cint.h"

#include "core/bezierpath.h"

#include "render/init.h"
#include "render/debuggl.h"
#include "render/materialshader.h"
#include "render/compiledmesh.h"
#include "render/texturecache.h"
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/world.h"
#include "render/viewport.h"
#include "render/materialshadercache.h"
#include "render/defaultfiles.h"


#include "window/imguilibrary.h"
#include "window/timer.h"
#include "window/imgui_extra.h"
// #include "window/fpscontroller.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/filesystem.h"
#include "window/engine.h"
#include "window/canvas.h"

#include "imgui/imgui.h"
#include "SDL.h"
#include <iostream>
#include <memory>


#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "window/imgui_extra.h"


using namespace euphoria::core;
using namespace euphoria::render;
using namespace euphoria::window;


int
main(int argc, char** argv)
{
    Engine engine;

    if (const auto r = engine.setup(argparse::NameAndArguments::extract(argc, argv)); r != 0)
    {
        return r;
    }


    int window_width = 1280;
    int window_height = 720;

    if(!engine.create_window("Painter", window_width, window_height, true))
    {
        return -1;
    }

    // viewport_handler viewport_handler;
    // viewport_handler.set_size(window_width, window_height);


    bool running = true;

    //////////////////////////////////////////////////////////////////////////////
    // main loop
    CanvasConfig cc;
    Canvas canvas;
    BezierPath2 path(Vec2f(0, 0));
    int index = -1;

    while(running)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            imgui::process_imgui_events(&e);

            if(engine.on_resize(e, &window_width, &window_height))
            {
                // viewport_handler.set_size(window_width, window_height);
            }

            switch(e.type)
            {
            case SDL_QUIT: running = false; break;
            default:
                // ignore other events
                break;
            }
        }

        engine.imgui->start_new_frame();

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

            if(ImGui::IsMouseReleased(0))
            {
                index = -1;
            }
            auto handle = [&canvas, &index](const ImVec2& p, int id, ImU32 color)
            {
                const auto size = 5.0f;
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
                draw_list->AddCircleFilled(sp, size, color);
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
            };
            auto line = [](const ImVec2& a, const ImVec2& b, ImU32 color)
            {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->PathLineTo(a);
                draw_list->PathLineTo(b);
                draw_list->PathStroke(color, ImDrawFlags_None);
            };

            const auto curve_color = IM_COL32(0, 0, 200, 255);
            const auto line_color = IM_COL32(0, 0, 0, 255);

            // draw handles
            for(size_t point_index = 0; point_index < path.points.size(); point_index += 1)
            {
                const bool is_anchor_point = BezierPath2::is_anchor_point(point_index);
                if(path.autoset && !is_anchor_point)
                {
                    continue;
                }
                const ImU32 alpha = 200;
                const ImU32 color = is_anchor_point
                    ? IM_COL32(20, 20, 200, alpha)
                    : IM_COL32(200, 20, 20, alpha)
                    ;
                auto r = handle(con(path.points[point_index]), euphoria::core::c_sizet_to_int(point_index), color);
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
            const auto tseg = path.get_number_of_segments();
            for(size_t seg = 0; seg < tseg; seg += 1)
            {
                auto s = path.get_points_in_segment(seg);
                auto* dl = ImGui::GetWindowDrawList();
                dl->AddBezierCubic
                (
                    canvas.world_to_screen(con(s.a0)),
                    canvas.world_to_screen(con(s.c0)),
                    canvas.world_to_screen(con(s.c1)),
                    canvas.world_to_screen(con(s.a1)),
                    curve_color,
                    1
                );

                if(!path.autoset)
                {
                    line(canvas.world_to_screen(con(s.a0)), canvas.world_to_screen(con(s.c0)), line_color);
                    line(canvas.world_to_screen(con(s.a1)), canvas.world_to_screen(con(s.c1)), line_color);
                }
            }

            canvas.show_ruler(cc);
            canvas.end(cc);

            if(ImGui::BeginPopup("context_menu"))
            {
                const auto p = canvas.screen_to_world(ImGui::GetMousePosOnOpeningCurrentPopup());
                if(ImGui::MenuItem("Add"))
                {
                    path.add_point(con(p));
                }
                auto ic = path.is_closed;
                if(ImGui::Checkbox("Is closed", &ic))
                {
                    path.toggle_closed();
                }
                auto as = path.autoset;
                if(ImGui::Checkbox("Autoset control points", &as))
                {
                    path.toggle_auto_set_control_points();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();

        // ImGui::ShowMetricsWindow();

        engine.init->clear_screen(NamedColor::light_gray);
        imgui::imgui_render();

        SDL_GL_SwapWindow(engine.window->window);
    }

    return 0;
}
