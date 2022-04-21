#include "log/log.h"

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
#include "core/cint.h"
#include "core/viewport.h"
#include "core/polybezier.h"

#include "render/init.h"
#include "render/debuggl.h"
#include "render/materialshader.h"
#include "render/compiledmesh.h"
#include "render/texturecache.h"
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/world.h"
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
#include "window/open_color.imgui.h"

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
    EUPH_INIT_LOGGING();
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
    PolyBezier2 path(Vec2f(0, 0));
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

        imgui::start_new_frame();

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
            auto handle = [&canvas, &index](const ImVec2& p, int id, ImU32 hover_color, ImU32 default_color)
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
            };
            auto line = [](const ImVec2& from, const ImVec2& to, ImU32 color)
            {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->PathLineTo(from);
                draw_list->PathLineTo(to);
                draw_list->PathStroke(color, ImDrawFlags_None);
            };

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
                auto r = handle(con(path.points[point_index]), euphoria::core::c_sizet_to_int(point_index), color[handle_hover_shade], color[handle_default_shade]);
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

                if(!path.is_autoset_enabled)
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
                auto as = path.is_autoset_enabled;
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
