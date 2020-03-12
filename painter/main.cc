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
#include "core/camera.h"
#include "core/stringutils.h"
#include "core/stdutils.h"
#include "core/proto.h"
#include "core/log.h"

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
#include "window/imgui_ext.h"
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
#include "window/imgui_ext.h"

// LOG_SPECIFY_DEFAULT_LOGGER("painter")

using namespace euphoria::core;
using namespace euphoria::render;
using namespace euphoria::window;

int
main(int argc, char** argv)
{
    Engine engine;

    if (const auto r = engine.Setup(argparse::Arguments::Extract(argc, argv)); r != 0)
    {
        return r;
    }


    int window_width  = 1280;
    int window_height = 720;

    if(!engine.CreateWindow("Painter", window_width, window_height, true))
    {
        return -1;
    }

    // ViewportHandler viewport_handler;
    // viewport_handler.SetSize(window_width, window_height);


    bool running = true;

    //////////////////////////////////////////////////////////////////////////////
    // main loop
    CanvasConfig cc;
    Canvas       canvas;
    BezierPath2  path(vec2f(0, 0));
    int          index = -1;

    while(running)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            engine.imgui->ProcessEvents(&e);

            if(engine.HandleResize(e, &window_width, &window_height))
            {
                // viewport_handler.SetSize(window_width, window_height);
            }

            switch(e.type)
            {
            case SDL_QUIT: running = false; break;
            default:
                // ignore other events
                break;
            }
        }

        engine.imgui->StartNewFrame();

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

            canvas.Begin(cc);
            canvas.ShowGrid(cc);

            if(ImGui::IsMouseReleased(0))
            {
                index = -1;
            }
            auto handle = [&canvas, &index](
                                  const ImVec2& p, int id, ImU32 color) {
                const auto  size      = 5.0f;
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                const auto  sp        = canvas.WorldToScreen(p);
                const auto  me        = ImGui::GetMousePos();
                const auto  hover
                        = vec2f::FromTo(C(me), C(sp)).GetLengthSquared()
                          < size * size;
                if(index == -1 && hover && ImGui::IsMouseDown(0))
                {
                    index = id;
                }
                draw_list->AddCircleFilled(sp, size, color);
                if(index == id)
                {
                    // capture current drag item...
                    if(ImGui::IsMouseDragging())
                    {
                        const auto d = ImGui::GetMouseDragDelta();
                        ImGui::ResetMouseDragDelta();
                        // todo: handle scale/zoom
                        return std::make_pair(
                                true, vec2f(d.x, d.y) / canvas.view.scale);
                    }
                }
                return std::make_pair(false, vec2f(0, 0));
            };
            auto line = [](const ImVec2& a, const ImVec2& b, ImU32 color) {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->PathLineTo(a);
                draw_list->PathLineTo(b);
                draw_list->PathStroke(color, false);
            };

            const auto curve_color = IM_COL32(0, 0, 200, 255);
            const auto line_color  = IM_COL32(0, 0, 0, 255);

            // draw handles
            for(size_t point_index = 0; point_index < path.points.size();
                point_index += 1)
            {
                const bool is_anchor_point
                        = BezierPath2::IsAnchorPoint(point_index);
                if(path.autoset_ && !is_anchor_point)
                {
                    continue;
                }
                const ImU32 alpha = 200;
                const ImU32 color = is_anchor_point
                                            ? IM_COL32(20, 20, 200, alpha)
                                            : IM_COL32(200, 20, 20, alpha);
                auto r = handle(
                        C(path.points[point_index]), point_index, color);
                if(r.first)
                {
                    if(ImGui::GetIO().KeyCtrl)
                    {
                        path.points[point_index] += r.second;
                    }
                    else
                    {
                        path.MovePoint(point_index, r.second);
                    }
                }
            }
            // draw bezier and link lines
            const auto tseg = path.GetNumberOfSegments();
            for(size_t seg = 0; seg < tseg; seg += 1)
            {
                auto  s  = path.GetPointsInSegment(seg);
                auto* dl = ImGui::GetWindowDrawList();
                dl->AddBezierCurve(
                        canvas.WorldToScreen(C(s.a0)),
                        canvas.WorldToScreen(C(s.c0)),
                        canvas.WorldToScreen(C(s.c1)),
                        canvas.WorldToScreen(C(s.a1)),
                        curve_color,
                        1);

                if(!path.autoset_)
                {
                    line(canvas.WorldToScreen(C(s.a0)),
                         canvas.WorldToScreen(C(s.c0)),
                         line_color);
                    line(canvas.WorldToScreen(C(s.a1)),
                         canvas.WorldToScreen(C(s.c1)),
                         line_color);
                }
            }

            canvas.ShowRuler(cc);
            canvas.End(cc);

            if(ImGui::BeginPopup("context_menu"))
            {
                const auto p = canvas.ScreenToWorld(
                        ImGui::GetMousePosOnOpeningCurrentPopup());
                if(ImGui::MenuItem("Add"))
                {
                    path.AddPoint(C(p));
                }
                auto ic = path.is_closed_;
                if(ImGui::Checkbox("Is closed", &ic))
                {
                    path.ToggleClosed();
                }
                auto as = path.autoset_;
                if(ImGui::Checkbox("Autoset control points", &as))
                {
                    path.ToggleAutoSetControlPoints();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();

        // ImGui::ShowMetricsWindow();

        engine.init->ClearScreen(Color::LightGray);
        engine.imgui->Render();

        SDL_GL_SwapWindow(engine.window->window);
    }

    return 0;
}
