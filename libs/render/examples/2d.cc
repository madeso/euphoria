#include "SDL.h"

#include "core/log.h"
#include "core/str.h"
#include "core/interpolate.h"
#include "core/os.h"
#include "core/ecs-systems.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_defaultshaders.h"
#include "core/proto.h"
#include "core/viewportdef.h"
#include "core/image_draw.h"

#include "render/debuggl.h"
#include "render/font.h"
#include "render/init.h"
#include "render/fontcache.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/texturecache.h"
#include "render/viewport.h"
#include "render/shader.h"
#include "render/spriterender.h"
#include "render/viewporthandler.h"

#include "window/key.h"
#include "window/imguilibrary.h"
#include "window/imgui_extra.h"
#include "window/filesystem.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/engine.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

LOG_SPECIFY_DEFAULT_LOGGER("samples-gui")


using namespace euphoria::core;
using namespace euphoria::render;
using namespace euphoria::window;


int
main(int argc, char* argv[])
{
    engine engine;
    if(const auto ret = engine.setup(argparse::name_and_arguments::extract(argc, argv)); ret != 0)
    {
        return ret;
    }

    engine.file_system->set_write_root
    (
        std::make_shared<vfs::write_root_physical_folder>(get_current_directory())
    );

    texture_cache cache {engine.file_system.get()};

    const auto clear_color = color::light_gray;

    int window_width  = 800;
    int window_height = 600;

    if
    (
        engine.create_window
        (
                "euphoria 2d demo",
                window_width,
                window_height,
                true
        ) == false
    )
    {
        return -1;
    }


    shader shader;
    attributes2d::prebind_shader(&shader);
    shader.load(engine.file_system.get(), vfs::file_path{"~/shaders/sprite"});
    sprite_renderer renderer(&shader);
    font_cache      font_cache {engine.file_system.get(), &cache};

    use(&shader);
    shader.set_uniform(shader.get_uniform("image"), 0);

    auto viewport_handler = euphoria::render::viewport_handler
    {
        engine.init.get(),
        nullptr
    };
    viewport_handler.add(&shader);
    viewport_handler.set_size(window_width, window_height);

    Uint64 now  = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    // SDL_StartTextInput();

    bool running = true;
    constexpr bool show_imgui = true;

    int window_mouse_x = 0;
    int window_mouse_y = 0;
    SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
    bool mouse_lmb_down = false;

    {
        image image;
        image.setup_no_alpha_support(256, 256);
        constexpr float h = 256.0f/2;
        constexpr float m = 256.0f;
        constexpr int s = 5;

        clear(&image, {color::white});
        draw_arrow(&image, vec2f{h, 0}, vec2f{h, m}, {color::blue}, s);
        draw_arrow(&image, vec2f{0, h}, vec2f{m, h}, {color::red}, s);

        engine.catalog->register_file_data
        (
            vfs::file_path{"~/image"},
            image.write(image_write_format::png)
        );
    }
    auto arrows = cache.get_texture(vfs::file_path{"~/image"});

    engine.init->use_2d();

    float sprite_width = 10;
    float sprite_height = 10;
    float sprite_x = 10;
    float sprite_y = 10;

    while(running)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        //const float dt = (now - last) * 1.0f / SDL_GetPerformanceFrequency();
        SDL_Event e;

        // imgui
        if(show_imgui)
        {
            engine.imgui->start_new_frame();

            ImGui::Begin("2d");
            ImGui::DragFloat("width", &sprite_width);
            ImGui::DragFloat("height", &sprite_height);
            ImGui::DragFloat("x", &sprite_x);
            ImGui::DragFloat("y", &sprite_y);
            ImGui::End();
        }

        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }

            if(show_imgui)
            {
                engine.imgui->process_events(&e);
            }

            if(engine.on_resize(e, &window_width, &window_height))
            {
                viewport_handler.set_size(window_width, window_height);
            }

            if(e.type == SDL_MOUSEMOTION)
            {
                window_mouse_x = e.motion.x;
                window_mouse_y = e.motion.y;
            }
            else if(e.type == SDL_KEYUP || e.type == SDL_KEYDOWN)
            {
                const bool down = e.type == SDL_KEYDOWN;
                const auto key = to_key(e.key.keysym);
                if(down && key == key::escape)
                {
                    running = false;
                }
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN
                    || e.type == SDL_MOUSEBUTTONUP)
            {
                const bool down = e.type == SDL_MOUSEBUTTONDOWN;
                window_mouse_x = e.button.x;
                window_mouse_y = e.button.y;
                if(e.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_lmb_down = down;
                }
            }
            else if(e.type == SDL_TEXTINPUT)
            {
                // const std::string& input = e.text.text;
            }
        }

        engine.init->clear_screen(clear_color);
        const auto r = rectf::from_position_anchor_width_and_height
        (
            vec2f{sprite_x, sprite_y},
            scale2f{0.5f, 0.5f},
            static_cast<float>(arrows->width),
            static_cast<float>(arrows->height)
        );
        /*
        const auto sr =
            Rectf::FromWidthHeight(sprite_width, sprite_height)
                .OffsetCopy(sprite_x, sprite_y)
            ;
        */
        renderer.draw_sprite
        (
            *arrows,
            r
        );
        if(show_imgui)
        {
            engine.imgui->render();
        }
        SDL_GL_SwapWindow(engine.window->window);
    }

    return 0;
}

