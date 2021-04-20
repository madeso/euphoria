#include "core/random.h"
#include "core/rgb.h"
#include "core/colors.h"
#include "core/palette_lospec.h"
#include "core/palette.h"
#include "core/image.h"
#include "core/cint.h"

#include "render/init.h"


#include "window/imguilibrary.h"
#include "window/imgui_ext.h"
#include "window/imgui_extra.h"
#include "window/imgui_icons.h"
#include "window/sdlwindow.h"
#include "window/engine.h"
#include "window/canvas.h"

#include "imgui/imgui.h"
#include "SDL.h"



#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "window/imgui_ext.h"


using namespace euphoria::core;
using namespace euphoria::window;


/*
PixLE todo

General:
brushes / sizes
brush preview/hover
transparency
tool: eraser
layers
undo/redo
save/load via terminal
fast draw:
algorithms listed by https://rickyhan.com/
aa lines
clone/pattern brush
line/rectangle/circle tools
add https://github.com/lendrick/Gradientifier/

Tileset mode:
tile x/y mode
draw om example tileset mode
functions to autocopy part of images

Sprite mode:
Sprite animations

*/


ImU32
C(const rgbai& c)
{
    return IM_COL32(c.r, c.g, c.b, c.a);
}


bool IsOver(const ImVec2& min, const ImVec2& max, const ImVec2& mouse)
{
    const auto over_min = min.x <= mouse.x && min.y <= mouse.y;
    const auto over_max = max.x >= mouse.x && max.y >= mouse.y;
    return over_min && over_max;
}


enum class Tool
{
    Pen, Fill
};


void
FloodFill(image* image, int x, int y, const rgbai& target_color, const rgbai& replacement_color)
{
    if(IsWithin(image->get_indices(), vec2i(x, y)) == false) { return; }
    if(target_color == replacement_color) { return; }
    if(image->get_pixel(x, y) != target_color) { return; }
    image->set_pixel(x, y, replacement_color);

    FloodFill(image, x-1, y, target_color, replacement_color);
    FloodFill(image, x+1, y, target_color, replacement_color);
    FloodFill(image, x, y-1, target_color, replacement_color);
    FloodFill(image, x, y+1, target_color, replacement_color);
}


int
main(int argc, char** argv)
{
    Engine engine;

    if (const auto r = engine.Setup(argparse::name_and_arguments::extract(argc, argv)); r != 0)
    {
        return r;
    }


    int window_width  = 1280;
    int window_height = 720;

    if(!engine.CreateWindow("PixLE", window_width, window_height, true))
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
    image image;
    Random random;
    Tool tool = Tool::Pen;
    auto palette = palettes::EDG64();
    auto foreground = 0;
    auto background = 1;

    image.setup_no_alpha_support(64, 64);
    image.set_all_top_bottom([&](int, int)
    {
        return rgbai{palette.colors[background]};
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
        const auto menu_height = ImGui::GetCurrentWindow()->MenuBarHeight();
        ImGui::EndMainMenuBar();

        const auto toolbar_height = 30;
        const auto toolbar_button_size = ImVec2(0, 20);

        ImGui::SetNextWindowPos(ImVec2(0, 0 + menu_height), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, toolbar_height), ImGuiCond_Always);
        const auto toolbar_flags = 0
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings
            ;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
        const auto show_toolbar = ImGui::Begin("TOOLBAR", NULL, toolbar_flags);
        ImGui::PopStyleVar(2);
        if(show_toolbar)
        {
            auto toolbar_button = [&tool, toolbar_button_size](const char* label, Tool t)
            {
                if(ToggleButton(label, tool == t, toolbar_button_size))
                {
                    tool = t;
                }
            };
            toolbar_button(ICON_MDI_FORMAT_COLOR_HIGHLIGHT, Tool::Pen);
            HelpText("Pen");
            
            ImGui::SameLine();
            toolbar_button(ICON_MDI_FORMAT_COLOR_FILL, Tool::Fill);
            HelpText("Fill");
        }
        ImGui::End();

        if(ImGui::Begin("Palette"))
        {

            const auto tile_size = 20;
            const auto spacing = 3;
            const auto big_spacing = 10;
            const auto big_offset = 0.20f;
            const auto max_pal_size = tile_size * 5;

            if(imgui::CanvasBegin(ImVec4(0.3, 0.3, 0.3, 1.0f), "palette"))
            {
                const auto p = ImGui::GetCursorScreenPos();
                const auto size = ImGui::GetContentRegionAvail();

                const auto hovering = ImGui::IsAnyItemHovered();
                const auto left_clicked = ImGui::IsMouseClicked(0);
                const auto right_clicked = ImGui::IsMouseClicked(1);
                const auto mouse = ImGui::GetMousePos();

                ImDrawList* draw_list = ImGui::GetWindowDrawList();

                auto x = 0.0f;
                auto y = 0.0f;

                for(int palette_index=0; palette_index<Csizet_to_int(palette.colors.size()); palette_index+=1)
                {
                    if(x + tile_size > size.x)
                    {
                        x = 0;
                        y += tile_size + spacing;
                    }

                    const auto min = p + ImVec2(x, y);
                    const auto max = min + ImVec2(tile_size, tile_size);

                    draw_list->AddRectFilled(min, max, C(palette.colors[palette_index]));
                    x += tile_size + spacing;

                    if(!hovering && (left_clicked || right_clicked))
                    {
                        if(IsOver(min, max, mouse))
                        {
                            if(left_clicked)
                            {
                                foreground = palette_index;
                            }
                            else
                            {
                                background = palette_index;
                            }
                            
                        }
                    }
                }

                y += tile_size;

                const auto big_size = std::min(size.x, size.y - y) - big_spacing * 2;

                if(big_size > 0)
                {
                    const auto bsf = std::min<float>(max_pal_size, big_size / (1+big_offset));
                    const auto bs = ImVec2(bsf, bsf);
                    const auto foreground_pos = ImVec2
                    (
                        size.x/2 - (bsf * (1+big_offset))/2,
                        y+big_spacing
                    );

                    const auto background_pos = foreground_pos + bs * big_offset;
                    draw_list->AddRectFilled(p + background_pos, p + background_pos + bs, C(palette.get_safe_index(background)));
                    draw_list->AddRectFilled(p + foreground_pos, p + foreground_pos + bs, C(palette.get_safe_index(foreground)));
                }

                imgui::CanvasEnd();
            }
        }
        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2 {300, 300}, ImGuiCond_FirstUseEver);
        if(ImGui::Begin("Image"))
        {
            if(image.is_valid())
            {
                const auto hovering = ImGui::IsAnyItemHovered();
                const auto left_down = ImGui::IsMouseDown(0);
                const auto right_down = ImGui::IsMouseDown(1);

                const auto left_clicked = ImGui::IsMouseClicked(0);
                const auto right_clicked = ImGui::IsMouseClicked(1);

                canvas.Begin(cc);
                canvas.ShowGrid(cc);

                // draw image
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                image.for_all_top_bottom([&](int x, int y, const rgbai& c)
                {
                    const auto pixel_size = 5;
                    const auto p = ImVec2(x * pixel_size, y*pixel_size);
                    const auto s = ImVec2(pixel_size, pixel_size);
                    const auto ps = canvas.WorldToScreen(p);
                    const auto pss = canvas.WorldToScreen(p+s);
                    const auto m = ImGui::GetMousePos();
                    if(ps.x <= m.x && ps.y <= m.y && pss.x >= m.x && pss.y >= m.y)
                    {
                        switch(tool)
                        {
                        case Tool::Pen:
                            // hovering over pixel
                            if(!hovering && left_down)
                            {
                                image.set_pixel(x, y, palette.colors[foreground]);
                            }

                            // hovering over pixel
                            if(!hovering && right_down)
                            {
                                image.set_pixel(x, y, palette.colors[background]);
                            }
                            break;
                        case Tool::Fill:
                            if(!hovering && (left_clicked || right_clicked))
                            {
                                const auto color = palette.colors[left_clicked ? foreground : background];
                                FloodFill(&image, x, y, c, color);
                            }
                        }
                        
                    }
                    draw_list->AddRectFilled(ps, pss, C(c));
                });

                canvas.ShowRuler(cc);
                canvas.End(cc);
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
