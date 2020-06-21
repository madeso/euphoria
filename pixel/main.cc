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
#include "core/rgb.h"
#include "core/colors.h"
#include "core/palette_lospec.h"
#include "core/palette.h"

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
#include "window/imgui_extra.h"
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


using namespace euphoria::core;
using namespace euphoria::render;
using namespace euphoria::window;


ImU32
C(const Rgbai& c)
{
    return IM_COL32(c.r, c.g, c.b, c.a);
}


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
    Canvas canvas;
    Image image;
    Random random;
    auto palette = palette::EDG64();
    auto foreground = 0;
    auto background = 1;

    image.SetupNoAlphaSupport(64, 64);
    image.SetAllTopBottom([&](int x, int y)
    {
        return Rgbai{palette.colors[background]};
    });

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

        if(ImGui::Begin("palette"))
        {
            const auto tile_size = 20;
            const auto spacing = 3;

            auto x = 0.0f;
            auto y = 0.0f;

            if(imgui::CanvasBegin(ImVec4(0.3, 0.3, 0.3, 1.0f), "palette"))
            {
                const auto p = ImGui::GetCursorScreenPos();
                const auto size = ImGui::GetContentRegionAvail();

                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                for(int i=0; i<palette.colors.size(); i+=1)
                {
                    if(x + tile_size > size.x)
                    {
                        x = 0;
                        y += tile_size + spacing;
                    }

                    const auto min = p + ImVec2(x, y);
                    const auto max = min + ImVec2(tile_size, tile_size);

                    draw_list->AddRectFilled(min, max, C(palette.colors[i]));
                    x += tile_size + spacing;
                }
                imgui::CanvasEnd();
            }
        }
        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2 {300, 300}, ImGuiCond_FirstUseEver);
        if(ImGui::Begin("pixel"))
        {
            if(image.IsValid())
            {
                const auto hovering = ImGui::IsAnyItemHovered();
                const auto left_down = ImGui::IsMouseDown(0);
                const auto right_clicked = ImGui::IsMouseClicked(1);
                if(!hovering && right_clicked)
                {
                    ImGui::OpenPopup("context_menu");
                }

                canvas.Begin(cc);
                canvas.ShowGrid(cc);

                // draw image
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                image.ForAllTopBottom([&](int x, int y, const Rgbai& c)
                {
                    const auto pixel_size = 5;
                    const auto p = ImVec2(x * pixel_size, y*pixel_size);
                    const auto s = ImVec2(pixel_size, pixel_size);
                    const auto ps = canvas.WorldToScreen(p);
                    const auto pss = canvas.WorldToScreen(p+s);
                    const auto m = ImGui::GetMousePos();
                    if(ps.x <= m.x && ps.y <= m.y && pss.x >= m.x && pss.y >= m.y)
                    {
                        // hovering over pixel
                        if(!hovering && left_down)
                        {
                            image.SetPixel(x, y, palette.colors[foreground]);
                        }
                    }
                    draw_list->AddRectFilled(ps, pss, C(c));
                });

                canvas.ShowRuler(cc);
                canvas.End(cc);

                if(ImGui::BeginPopup("context_menu"))
                {
                    // const auto p = canvas.ScreenToWorld(ImGui::GetMousePosOnOpeningCurrentPopup());
                    if(ImGui::MenuItem("Add"))
                    {
                    }
                    ImGui::EndPopup();
                }
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
