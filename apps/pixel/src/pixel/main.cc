#include "log/log.h"

#include "core/random.h"
#include "core/rgb.h"
#include "core/colors.h"
#include "core/palette_lospec.h"
#include "core/palette.h"
#include "core/image.h"
#include "core/cint.h"

#include "render/init.h"


#include "window/imguilibrary.h"
#include "window/imgui_extra.h"
#include "window/imgui_icons.h"
#include "window/sdlwindow.h"
#include "window/engine.h"
#include "window/canvas.h"
#include "window/app.h"

#include "imgui/imgui.h"
#include "SDL.h"



#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "window/imgui_extra.h"


using namespace euphoria;
using namespace euphoria::core;
using namespace euphoria::window;



ImU32
con(const Rgbai& c)
{
    return IM_COL32(c.r, c.g, c.b, c.a);
}


bool is_over(const ImVec2& min, const ImVec2& max, const ImVec2& mouse)
{
    const auto over_min = min.x <= mouse.x && min.y <= mouse.y;
    const auto over_max = max.x >= mouse.x && max.y >= mouse.y;
    return over_min && over_max;
}


enum class Tool
{
    pen, fill
};


void
flood_fill(Image* image, int x, int y, const Rgbai& target_color, const Rgbai& replacement_color)
{
    if(is_within(image->get_indices(), vec2i(x, y)) == false) { return; }
    if(target_color == replacement_color) { return; }
    if(image->get_pixel(x, y) != target_color) { return; }

    image->set_pixel(x, y, replacement_color);

    flood_fill(image, x - 1, y, target_color, replacement_color);
    flood_fill(image, x + 1, y, target_color, replacement_color);
    flood_fill(image, x, y - 1, target_color, replacement_color);
    flood_fill(image, x, y + 1, target_color, replacement_color);
}


struct PixelApp : App
{
    CanvasConfig cc;
    Canvas canvas;
    Image image;
    core::Random random;
    Tool current_tool = Tool::pen;
    Palette palette = *palettes::lospec::endesga_64;
    int foreground = 0;
    int background = 1;

    PixelApp()
    {
        image.setup_no_alpha_support(64, 64);
        image.set_all_top_bottom([&](int, int)
        {
            return Rgbai{palette.colors[background]};
        });
    }

    static constexpr int toolbar_height = 30;
    ImVec2 toolbar_button_size = ImVec2(0, 20);

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
        const auto menu_height = ImGui::GetCurrentWindow()->MenuBarHeight();
        ImGui::EndMainMenuBar();

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
        const auto show_toolbar = ImGui::Begin("TOOLBAR", nullptr, toolbar_flags);
        ImGui::PopStyleVar(2);
        if(show_toolbar)
        {
            auto toolbar_button = [this](const char* label, ::Tool t)
            {
                if(imgui::imgui_toggle_button(label, current_tool == t, toolbar_button_size))
                {
                    current_tool = t;
                }
            };
            toolbar_button(ICON_MD_BRUSH, Tool::pen);
            imgui::add_help_text_for_previous_widget("Pen");

            ImGui::SameLine();
            toolbar_button(ICON_MD_FORMAT_COLOR_FILL, Tool::fill);
            imgui::add_help_text_for_previous_widget("Fill");
        }
        ImGui::End();

        if(ImGui::Begin("Palette"))
        {
            const auto tile_size = 20;
            const auto spacing = 3;
            const auto big_spacing = 10;
            const auto big_offset = 0.20f;
            const auto max_pal_size = tile_size * 5;

            if(imgui::begin_canvas_widget(ImVec4(0.3f, 0.3f, 0.3f, 1.0f), "palette"))
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

                for(int palette_index=0; palette_index < c_sizet_to_int(palette.colors.size()); palette_index+=1)
                {
                    if(x + tile_size > size.x)
                    {
                        x = 0;
                        y += tile_size + spacing;
                    }

                    const auto min = p + ImVec2(x, y);
                    const auto max = min + ImVec2(tile_size, tile_size);

                    draw_list->AddRectFilled(min, max, con(palette.colors[palette_index]));
                    x += tile_size + spacing;

                    if(!hovering && (left_clicked || right_clicked))
                    {
                        if(is_over(min, max, mouse))
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
                    draw_list->AddRectFilled(p + background_pos, p + background_pos + bs, con(palette.get_safe_index(background)));
                    draw_list->AddRectFilled(p + foreground_pos, p + foreground_pos + bs, con(palette.get_safe_index(foreground)));
                }

                imgui::end_canvas_widget();
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

                canvas.begin(cc);
                canvas.show_grid(cc);

                // draw image
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                image.run_for_all_top_bottom([&](int x, int y, const Rgbai& c)
                {
                    const auto pixel_size = 5;
                    const auto p = ImVec2(core::c_int_to_float(x * pixel_size), core::c_int_to_float(y*pixel_size));
                    const auto s = ImVec2(pixel_size, pixel_size);
                    const auto ps = canvas.from_world_to_screen(p);
                    const auto pss = canvas.from_world_to_screen(p + s);
                    const auto m = ImGui::GetMousePos();
                    if(ps.x <= m.x && ps.y <= m.y && pss.x >= m.x && pss.y >= m.y)
                    {
                        switch(current_tool)
                        {
                        case Tool::pen:
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
                        case Tool::fill:
                            if(!hovering && (left_clicked || right_clicked))
                            {
                                const auto color = palette.colors[left_clicked ? foreground : background];
                                flood_fill(&image, x, y, c, color);
                            }
                        }

                    }
                    draw_list->AddRectFilled(ps, pss, con(c));
                });

                canvas.show_ruler(cc);
                canvas.end(cc);
            }
        }
        ImGui::End();
    }
};


int
main(int argc, char** argv)
{
    return run_app(argc, argv, "PixLE", [] { return std::make_unique<PixelApp>();});
}
