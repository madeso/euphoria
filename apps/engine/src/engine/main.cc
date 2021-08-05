#include "SDL.h"
#include <iostream>

#include "core/log.h"
#include "core/str.h"
#include "core/interpolate.h"
#include "core/os.h"
#include "core/cint.h"
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
#include "window/imgui_extra.h"
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
#include "gaf_rapidjson_game.h"

using namespace euphoria::core;
using namespace euphoria::core::ecs;
using namespace euphoria::render;
using namespace euphoria::window;
using namespace euphoria::engine;


game::Game
load_game_data(vfs::file_system* fs)
{
    game::Game game;
    const auto err = read_json_to_gaf_struct_or_get_error_message(fs, &game, vfs::file_path{"~/gamedata.json"});
    if(!err.empty())
    {
        LOG_ERROR("Failed to load gamedata.json: {0}", err);
    }
    return game;
}


struct run_result
{
    bool ok;
    std::string message;

    [[nodiscard]]
    static
    run_result
    create_ok()
    {
        return run_result {true, ""};
    }

    [[nodiscard]]
    static
    run_result
    create_error(const std::string& message)
    {
        return run_result {false, message};
    }

private:
    run_result(bool b, const std::string& str) : ok(b), message(str) {}
};

run_result
run_main_script_file(lua* duk, vfs::file_system* fs, const vfs::file_path& path)
{
    std::string content;
    const bool loaded = fs->read_file_to_string(path, &content);
    if(!loaded)
    {
        const std::string error_message = string_builder() << "Unable to open " << path
                                                << " for running";
        LOG_ERROR("{0}", error_message);
        return run_result::create_error(error_message);
    }
    const auto eval = duk->state.script
    (
        content,
        [](lua_State*, sol::protected_function_result pfr) { return pfr; }
    );
    if(!eval.valid())
    {
        const sol::error err = eval;
        const std::string error_message = string_builder() << "Failed to run " << path << ": " << err.what();
        LOG_ERROR("{0}", error_message);
        return run_result::create_error(error_message);
    }

    return run_result::create_ok();
}


viewport_type
con(game::ViewportType type)
{
    switch(type)
    {
    case game::ViewportType::FitWithBlackBars:
        return viewport_type::fit_with_black_bars;
    case game::ViewportType::ScreenPixel:
        return viewport_type::screen_pixel;
    case game::ViewportType::Extend:
        return viewport_type::extend;
    default:
        DIE("Unhandled viewport case");
        return viewport_type::screen_pixel;
    }
}


rgb
get_color(std::shared_ptr<game::Color> c)
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


int custom_lua_exception_handler
(
    lua_State* lua_state,
    sol::optional<const std::exception&>,
    sol::string_view description
)
{
    LOG_ERROR("An exception occurred in a function: {0}", description);

	// note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
	// so we push a single string (in our case, the description of the error)
	return sol::stack::push(lua_state, description);
}

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

    game::Game gamedata = load_game_data(engine.file_system.get());
    const auto clear_color = get_color(gamedata.clear_color);

    int window_width = 800;
    int window_height = 600;

    if
    (
        engine.create_window
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

    input_system input;

    for(const auto& bind: gamedata.binds)
    {
        auto key = to_key(bind.key);
        if(key == key::invalid)
        {
            LOG_ERROR("Invalid key: {0}", bind.key);
            key = key::unbound;
        }

        input.add(std::make_shared<bound_var>(bind.name, key));
    }

    shader shader;
    attributes2d::prebind_shader(&shader);
    shader.load(engine.file_system.get(), vfs::file_path{"~/shaders/sprite"});
    sprite_renderer renderer(&shader);
    font_cache font_cache {engine.file_system.get(), &cache};

    lua duk;

    // todo(Gustav): replace with duk reference
    std::string& crash_message_string = duk.error;
    bool& has_crashed = duk.has_error;

    auto crash_on_exception = [&](const std::exception& ex)
    {
        has_crashed = true;
        crash_message_string = ex.what();
        LOG_ERROR("{0}", crash_message_string);
    };

    duk.state.set_exception_handler(&custom_lua_exception_handler);
    duk.state.open_libraries(sol::lib::base, sol::lib::package);
    add_print(&duk);
    bind_math(&duk);
    input_system::bind(&duk);

    systems systems;
    world world {&systems};
    components components {&world.reg};
    add_systems(&systems, &components);
    object_creator templates;
    load_templates_but_only_names(gamedata, &templates);
    camera_data camera_data;

    auto integration = script_integration
    {
        &systems,
        &world,
        &duk,
        &templates,
        &components,
        &camera_data
    };
    const auto error_run_main
            = run_main_script_file(&duk, engine.file_system.get(), vfs::file_path{"~/main.lua"});
    if(!error_run_main.ok)
    {
        has_crashed = true;
        crash_message_string = error_run_main.message;
    }
    load_templates
    (
        gamedata,
        &templates,
        &integration.get_registry(),
        &cache,
        &components
    );

    use(&shader);
    shader.set_uniform(shader.get_uniform("image"), 0);

    auto viewport_handler = euphoria::render::viewport_handler
    {
        engine.init.get(),
        &camera_data.screen
    };
    viewport_handler.add(&shader);
    viewport_handler.type = con(gamedata.viewport.type);
    viewport_handler.virtual_width = gamedata.viewport.width;
    viewport_handler.virtual_height = gamedata.viewport.height;

    viewport_handler.set_size(window_width, window_height);

    try
    {
        load_world
        (
            engine.file_system.get(),
            &world,
            &integration.get_registry(),
            vfs::file_path{"~/world.json"},
            &templates,
            &duk
        );
    }
    catch(const std::exception& ex)
    {
        crash_on_exception(ex);
    }

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    SDL_StartTextInput();

    bool running = true;

    int window_mouse_x = 0;
    int window_mouse_y = 0;
    SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
    bool mouse_lmb_down = false;

    integration.bind_keys(input);

    engine.init->use_2d();

    auto handle_events = [&]()
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }
            if(engine.on_resize(e, &window_width, &window_height))
            {
                viewport_handler.set_size(window_width, window_height);
            }

            if(has_crashed)
            {
                imgui::process_imgui_events(&e);
                if(e.type == SDL_KEYUP)
                {
                    const auto key = to_key(e.key.keysym);
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
                    const auto key = to_key(e.key.keysym);
                    input.set_key_state(key, down ? 1.0f : 0.0f);
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
    };

    while(running)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        const float dt = euphoria::core::c_u64_to_float(now - last) / euphoria::core::c_u64_to_float(SDL_GetPerformanceFrequency());

        handle_events();
        engine.imgui->start_new_frame();

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

        input.update_state();


        if(has_crashed == false)
        {
            integration.bind_keys(input);
        }

        viewport_handler.clear_black();

        if(has_crashed)
        {
            // todo(Gustav): fix crash rendering, perhaps by using a debug gui or imgui
            // nothing much is required just a better overflow detection
            // when rendering the error, perhaps making the error more visible
            // though clicking around and debugging might be useful...
            engine.init->clear_screen(color::cornflower_blue);

            if(imgui::begin_fixed_overlay(corner::center, "Crashed"))
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
            engine.init->clear_screen(clear_color);
            world.draw(&renderer);
        }

        imgui::imgui_render();
        SDL_GL_SwapWindow(engine.window->window);

        world.reg.remove_entities_tagged_for_removal();
    }

    return 0;
}
