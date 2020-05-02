#include "SDL.h"

#include "core/log.h"
#include "core/str.h"
#include "core/interpolate.h"
#include "core/os.h"
#include "core/ecs-systems.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_defaultshaders.h"
#include "core/proto.h"
#include "core/viewport.h"
#include "core/image_draw.h"

#include "render/debuggl.h"
#include "render/fonts.h"
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
#include "window/imgui_ext.h"
#include "window/filesystem.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/engine.h"

#include "engine/loadworld.h"
#include "engine/systems.h"
#include "engine/dukintegration.h"
#include "engine/dukmathbindings.h"
#include "engine/dukprint.h"
#include "engine/input.h"
#include "engine/objectemplate.h"
#include "engine/components.h"
#include "engine/cameradata.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

LOG_SPECIFY_DEFAULT_LOGGER("samples-gui")


using namespace euphoria::core;
using namespace euphoria::render;
using namespace euphoria::window;
using namespace euphoria::engine;


int
main(int argc, char* argv[])
{
    Engine engine;
    if(const auto ret = engine.Setup(argparse::Arguments::Extract(argc, argv)) != 0; ret != 0)
    {
        return ret;
    }

    engine.file_system->SetWrite
    (
        std::make_shared<vfs::FileSystemWriteFolder>(GetCurrentDirectory())
    );

    TextureCache cache {engine.file_system.get()};

    const auto clear_color = Color::LightGray;

    ViewportHandler viewport_handler;

    int window_width  = 800;
    int window_height = 600;

    if
    (
        engine.CreateWindow
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

    viewport_handler.SetSize(window_width, window_height);

    Shader shader;
    attributes2d::PrebindShader(&shader);
    shader.Load(engine.file_system.get(), vfs::FilePath{"~/shaders/sprite"});
    SpriteRenderer renderer(&shader);
    FontCache      font_cache {engine.file_system.get(), &cache};

    Use(&shader);
    shader.SetUniform(shader.GetUniform("image"), 0);

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
        Image image;
        image.SetupNoAlphaSupport(256, 256);
        constexpr float h = 256.0f/2;
        constexpr float m = 256.0f;
        constexpr int s = 5;

        Clear(&image, {Color::White});
        DrawArrow(&image, vec2f{h, 0}, vec2f{h, m}, {Color::Blue}, s);
        DrawArrow(&image, vec2f{0, h}, vec2f{m, h}, {Color::Red}, s);

        engine.catalog->RegisterFileData
        (
            vfs::FilePath{"~/image"},
            image.Write(ImageWriteFormat::PNG)
        );
    }
    auto arrows = cache.GetTexture(vfs::FilePath{"~/image"});

    engine.init->Use2d();

    float sprite_width = 10;
    float sprite_height = 10;
    float sprite_x = 10;
    float sprite_y = 10;

    while(running)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        const float dt = (now - last) * 1.0f / SDL_GetPerformanceFrequency();
        SDL_Event e;

        // imgui
        if(show_imgui)
        {
            engine.imgui->StartNewFrame();

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
                engine.imgui->ProcessEvents(&e);
            }

            if(engine.HandleResize(e, &window_width, &window_height))
            {
                viewport_handler.SetSize(window_width, window_height);
            }

            if(e.type == SDL_MOUSEMOTION)
            {
                window_mouse_x = e.motion.x;
                window_mouse_y = e.motion.y;
            }
            else if(e.type == SDL_KEYUP || e.type == SDL_KEYDOWN)
            {
                const bool down = e.type == SDL_KEYDOWN;
                const auto key = ToKey(e.key.keysym);
                if(down && key == Key::ESCAPE)
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
        
        engine.init->ClearScreen(clear_color);
        const auto r = Rectf::FromPositionAnchorWidthAndHeight
        (
            vec2f{sprite_x, sprite_y},
            scale2f{0.5f, 0.5f},
            arrows->GetWidth(),
            arrows->GetHeight()
        );
        const auto sr = 
            Rectf::FromWidthHeight(sprite_width, sprite_height)
                .OffsetCopy(sprite_x, sprite_y)
            ;
        renderer.DrawSprite
        (
            *arrows,
            r
        );
        if(show_imgui)
        {
            engine.imgui->Render();
        }
        SDL_GL_SwapWindow(engine.window->window);
    }

    return 0;
}

