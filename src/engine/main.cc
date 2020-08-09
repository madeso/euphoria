#include "SDL.h"
#include <iostream>

#include "core/log.h"
#include "core/str.h"
#include "core/interpolate.h"
#include "core/os.h"
#include "core/ecs-systems.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_defaultshaders.h"
#include "core/proto.h"
#include "core/viewportdef.h"

#include "duk/duk.h"

#include "render/debuggl.h"
#include "render/font.h"
#include "render/init.h"
#include "render/fontcache.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/texturecache.h"
#include "render/viewport.h"
#include "render/viewporthandler.h"
#include "render/shader.h"
#include "render/spriterender.h"

#include "gui/root.h"

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

#include "gaf_game.h"


LOG_SPECIFY_DEFAULT_LOGGER("engine")


using namespace euphoria::core;
using namespace euphoria::core::ecs;
using namespace euphoria::duk;
using namespace euphoria::render;
using namespace euphoria::window;
using namespace euphoria::engine;
namespace duk = euphoria::duk;


game::Game
LoadGameData(vfs::FileSystem* fs)
{
    game::Game game;
    const auto err = LoadProtoJson(fs, &game, vfs::FilePath{"~/gamedata.json"});
    if(!err.empty())
    {
        LOG_ERROR("Failed to load gamedata.json: {0}", err);
    }
    return game;
}


struct RunResult
{
    bool        ok;
    std::string message;

    [[nodiscard]]
    static
    const RunResult
    Ok()
    {
        return RunResult {true, ""};
    }

    [[nodiscard]]
    static
    const RunResult
    Error(const std::string& message)
    {
        return RunResult {false, message};
    }

private:
    RunResult(bool b, const std::string& str) : ok(b), message(str) {}
};

RunResult
RunMainScriptFile(duk::Duk* duk, vfs::FileSystem* fs, const vfs::FilePath& path)
{
    std::string content;
    const bool  loaded = fs->ReadFileToString(path, &content);
    if(!loaded)
    {
        const std::string error_message = Str() << "Unable to open " << path
                                                << " for running";
        LOG_ERROR("{0}", error_message);
        return RunResult::Error(error_message);
    }
    std::string error;
    const bool  eval = duk->EvalString(content, path.path, &error, nullptr);
    if(!eval)
    {
        const std::string error_message = Str() << "Failed to run " << path
                                                << ": " << error;
        LOG_ERROR("{0}", error_message);
        return RunResult::Error(error_message);
    }

    return RunResult::Ok();
}


ViewportType
C(game::ViewportType type)
{
    switch(type)
    {
    case game::ViewportType::FitWithBlackBars:
        return ViewportType::FitWithBlackBars;
    case game::ViewportType::ScreenPixel:
        return ViewportType::ScreenPixel;
    case game::ViewportType::Extend:
        return ViewportType::Extend;
    default:
        DIE("Unhandled viewport case");
        return ViewportType::ScreenPixel;
    }
}


Rgb
GetColor(std::shared_ptr<game::Color> c)
{
    if(c == nullptr)
    {
        return Color::Gray;
    }

    if(c->hex != nullptr)
    {
        return Rgb::FromHex(colorutil::FromStringToHex(*c->hex));
    }

    LOG_ERROR("Unable to parse color");
    return Color::CornflowerBlue;
}


// engine


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

    game::Game gamedata = LoadGameData(engine.file_system.get());
    const auto clear_color = GetColor(gamedata.clear_color);

    int window_width  = 800;
    int window_height = 600;

    if
    (
        engine.CreateWindow
        (
            gamedata.title,
            window_width,
            window_height,
            true
        ) == false
    )
    {
        return -1;
    }

    // todo: update theese during runtime
    std::string crash_message_string;
    bool has_crashed = false;

    auto crash_on_exception = [&](const std::exception& ex)
    {
        has_crashed = true;
        crash_message_string = ex.what();
        LOG_ERROR("{0}", crash_message_string);
    };

    Input input;

    for(const auto& bind: gamedata.binds)
    {
        auto key = ToKey(bind.key);
        if(key == Key::INVALID)
        {
            LOG_ERROR("Invalid key: {0}", bind.key);
            key = Key::UNBOUND;
        }

        input.Add(std::make_shared<BoundVar>(bind.name, key));
    }

    Shader shader;
    attributes2d::PrebindShader(&shader);
    shader.Load(engine.file_system.get(), vfs::FilePath{"~/shaders/sprite"});
    SpriteRenderer renderer(&shader);
    FontCache      font_cache {engine.file_system.get(), &cache};

    // Sprite player(cache.GetTexture("player.png"));
    // objects.Add(&player);

    duk::Duk duk;
    AddPrint(&duk);
    BindMath(&duk);
    Input::Bind(&duk);

    Systems    systems;
    World      world {&systems};
    Components components {&world.reg};
    AddSystems(&systems, &components);
    ObjectCreator templates;
    LoadTemplatesButOnlyNames(gamedata, &templates);
    CameraData camera_data;

    auto integration = DukIntegration
    {
        &systems,
        &world,
        &duk,
        &templates,
        &components,
        &camera_data
    };
    const auto error_run_main
            = RunMainScriptFile(&duk, engine.file_system.get(), vfs::FilePath{"~/main.js"});
    if(!error_run_main.ok)
    {
        has_crashed          = true;
        crash_message_string = error_run_main.message;
    }
    LoadTemplates
    (
        gamedata,
        &templates,
        &integration.Registry(),
        &cache,
        &components
    );

    Use(&shader);
    shader.SetUniform(shader.GetUniform("image"), 0);

    auto viewport_handler = ViewportHandler
    {
        engine.init.get(),
        &camera_data.screen
    };
    viewport_handler.Add(&shader);
    viewport_handler.type = C(gamedata.viewport.type);
    viewport_handler.virtual_width = gamedata.viewport.width;
    viewport_handler.virtual_height = gamedata.viewport.height;

    viewport_handler.SetSize(window_width, window_height);

    try
    {
        LoadWorld
        (
            engine.file_system.get(),
            &world,
            &integration.Registry(),
            vfs::FilePath{"~/world.json"},
            &templates,
            duk.AsContext(),
            &duk
        );
    }
    catch(const std::exception& ex)
    {
        crash_on_exception(ex);
    }

    Uint64 now  = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    SDL_StartTextInput();

    bool running = true;

    int window_mouse_x = 0;
    int window_mouse_y = 0;
    SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
    bool mouse_lmb_down = false;

    integration.BindKeys(&duk, input);

    engine.init->Use2d();

    while(running)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        const float dt = (now - last) * 1.0f / SDL_GetPerformanceFrequency();
        SDL_Event e;

        engine.imgui->StartNewFrame();

        if(!has_crashed)
        {
            try
            {
                world.Update(dt);
            }
            catch(const std::exception& ex)
            {
                crash_on_exception(ex);
            }
        }

        input.UpdateState();

        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }
            if(engine.HandleResize(e, &window_width, &window_height))
            {
                viewport_handler.SetSize(window_width, window_height);
            }

            if(has_crashed)
            {
                engine.imgui->ProcessEvents(&e);
                if(e.type == SDL_KEYUP)
                {
                    const auto key = ToKey(e.key.keysym);
                    if(key == Key::ESCAPE)
                    {
                        running = false;
                    }
                }
            }
            else
            {
                if(e.type == SDL_MOUSEMOTION)
                {
                    window_mouse_x = e.motion.x;
                    window_mouse_y = e.motion.y;
                }
                else if(e.type == SDL_KEYUP || e.type == SDL_KEYDOWN)
                {
                    const bool down = e.type == SDL_KEYDOWN;
                    const auto key = ToKey(e.key.keysym);
                    input.SetKeyState(key, down ? 1.0f : 0.0f);
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
        }

        if(has_crashed == false)
        {
            integration.BindKeys(&duk, input);
        }

        viewport_handler.ClearBlack();

        if(has_crashed)
        {
            // todo: fix crash rendering, perhaps by using a debug gui or imgui
            // nothing much is required just a better overflow detection
            // when rendering the error, perhaps making the error more visible
            // though clicking around and debugging might be useful...
            engine.init->ClearScreen(Color::CornflowerBlue);

            if(BeginFixedOverlay(ImguiCorner::Center, "Crashed"))
            {
                ImGui::TextDisabled("%s", crash_message_string.c_str());
                if(ImGui::Button("Quit"))
                {
                    running = false;
                }
                ImGui::End();
            }
        }
        else
        {
            engine.init->ClearScreen(clear_color);
            world.Draw(&renderer);
        }

        engine.imgui->Render();
        SDL_GL_SwapWindow(engine.window->window);

        world.reg.RemoveRemoved();
    }

    return 0;
}
