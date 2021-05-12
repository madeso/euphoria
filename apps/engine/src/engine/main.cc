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

#include "core/sol.h"

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
using namespace euphoria::render;
using namespace euphoria::window;
using namespace euphoria::engine;


game::Game
LoadGameData(vfs::file_system* fs)
{
    game::Game game;
    const auto err = read_json_to_gaf_struct_or_get_error_message(fs, &game, vfs::file_path{"~/gamedata.json"});
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
RunMainScriptFile(Sol* duk, vfs::file_system* fs, const vfs::file_path& path)
{
    std::string content;
    const bool  loaded = fs->read_file_to_string(path, &content);
    if(!loaded)
    {
        const std::string error_message = string_builder() << "Unable to open " << path
                                                << " for running";
        LOG_ERROR("{0}", error_message);
        return RunResult::Error(error_message);
    }
    const auto eval = duk->lua.script
    (
        content,
        [](lua_State*, sol::protected_function_result pfr) { return pfr; }
    );
    if(!eval.valid())
    {
        const sol::error err = eval;
        const std::string error_message = string_builder() << "Failed to run " << path
                                                << ": " << err.what();
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


rgb
GetColor(std::shared_ptr<game::Color> c)
{
    if(c == nullptr)
    {
        return color::gray;
    }

    if(c->hex != nullptr)
    {
        return rgb::from_hex(colorutil::from_string_to_hex(*c->hex));
    }

    LOG_ERROR("Unable to parse color");
    return color::cornflower_blue;
}


// engine
int my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
	// L is the lua state, which you can wrap in a state_view if necessary
	// maybe_exception will contain exception, if it exists
	// description will either be the what() of the exception or a description saying that we hit the general-case catch(...)
	std::cout << "An exception occurred in a function, here's what it says ";
	if (maybe_exception) {
		std::cout << "(straight from the exception): ";
		const std::exception& ex = *maybe_exception;
		std::cout << ex.what() << std::endl;
	}
	else {
		std::cout << "(from the description parameter): ";
		std::cout.write(description.data(), description.size());
		std::cout << std::endl;
	}

	// you must push 1 element onto the stack to be
	// transported through as the error object in Lua
	// note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
	// so we push a single string (in our case, the description of the error)
	return sol::stack::push(L, description);
}

int
main(int argc, char* argv[])
{
    Engine engine;
    if(const auto ret = engine.Setup(argparse::name_and_arguments::extract(argc, argv)) != 0; ret != 0)
    {
        return ret;
    }

    engine.file_system->set_write_root
    (
        std::make_shared<vfs::write_root_physical_folder>(get_current_directory())
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

    Input input;

    for(const auto& bind: gamedata.binds)
    {
        auto key = to_key(bind.key);
        if(key == key::invalid)
        {
            LOG_ERROR("Invalid key: {0}", bind.key);
            key = key::unbound;
        }

        input.Add(std::make_shared<BoundVar>(bind.name, key));
    }

    Shader shader;
    attributes2d::PrebindShader(&shader);
    shader.Load(engine.file_system.get(), vfs::file_path{"~/shaders/sprite"});
    SpriteRenderer renderer(&shader);
    FontCache      font_cache {engine.file_system.get(), &cache};

    // Sprite player(cache.GetTexture("player.png"));
    // objects.Add(&player);

    Sol duk;

    // todo(Gustav): replace with duk reference
    std::string& crash_message_string = duk.error;
    bool& has_crashed = duk.has_error;

    auto crash_on_exception = [&](const std::exception& ex)
    {
        has_crashed = true;
        crash_message_string = ex.what();
        LOG_ERROR("{0}", crash_message_string);
    };

    duk.lua.set_exception_handler(&my_exception_handler);
    duk.lua.open_libraries(sol::lib::base, sol::lib::package);
    AddPrint(&duk);
    BindMath(&duk);
    Input::Bind(&duk);

    systems    systems;
    world      world {&systems};
    Components components {&world.reg};
    AddSystems(&systems, &duk, &components);
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
            = RunMainScriptFile(&duk, engine.file_system.get(), vfs::file_path{"~/main.lua"});
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
            vfs::file_path{"~/world.json"},
            &templates,
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
                world.update(dt);
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
                    if(key == key::escape)
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
            // todo(Gustav): fix crash rendering, perhaps by using a debug gui or imgui
            // nothing much is required just a better overflow detection
            // when rendering the error, perhaps making the error more visible
            // though clicking around and debugging might be useful...
            engine.init->ClearScreen(color::cornflower_blue);

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
            world.draw(&renderer);
        }

        engine.imgui->Render();
        SDL_GL_SwapWindow(engine.window->window);

        world.reg.remove_entities_tagged_for_removal();
    }

    return 0;
}
