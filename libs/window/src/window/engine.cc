#include "window/engine.h"

#include "SDL.h"
#include "imgui/imgui.h"

#include "base/os.h"
#include "log/log.h"
#include "io/vfs.h"
#include "core/vfs_defaultshaders.h"
#include "core/vfs_imagegenerator.h"

#include "render/defaultfiles.h"
#include "render/debuggl.h"
#include "render/init.h"

#include "window/filesystem.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/imguilibrary.h"

namespace eu::window
{
    Engine::Engine()
        : window_id(0)
    {
    }


    Engine::~Engine()
    {
        imgui.reset();
        init.reset();
        context.reset();
        window.reset();
        catalog.reset();
        file_system.reset();
        sdl.reset();
    }


    int
    Engine::setup(const core::argparse::NameAndArguments& args)
    {
        sdl = std::make_unique<SdlLibrary>();
        if(sdl->ok == false)
        {
            LOG_ERROR("Failed to create SDL");
            return -1;
        }

        auto parser = core::argparse::Parser("euphoria engine");

        auto current_directory = get_current_directory();
        parser
            .add("-w", &current_directory)
            .set_help("Sets the working direction if it's different from the current folder")
            ;
        const auto parse_result = parser.parse(args);
        if(parse_result != core::argparse::ok)
        {
            return parse_result.return_value;
        }

        LOG_INFO("Current directory: {0}", current_directory);

        file_system = std::make_unique<io::FileSystem>();
        catalog = io::ReadRootCatalog::create_and_add(file_system.get());
        io::ReadRootPhysicalFolder::add
        (
            file_system.get(),
            current_directory
        );
        core::ReadRootImageGenerator::add
        (
            file_system.get(),
            io::DirPath{"~/img-plain/"}
        );
        core::add_default_shaders
        (
            file_system.get(),
            io::DirPath{"~/shaders/"}
        );

        render::setup_default_files(catalog);

        return 0;
    }


    bool
    Engine::create_window
    (
        const std::string& title,
        int width,
        int height,
        bool blend_hack
    )
    {
        const auto pref_path = get_preference_path();

        window = std::make_unique<SdlWindow>(title, width, height, true);

        if(window->window == nullptr)
        {
            LOG_ERROR("Failed to create window {0}", SDL_GetError());
            return false;
        }

        window_id = SDL_GetWindowID(window->window);

        context = std::make_unique<SdlOpenglContext>(window.get());

        if(context->context == nullptr)
        {
            LOG_ERROR("Failed to create SDL context {0}", SDL_GetError());
            return false;
        }

        init = std::make_unique<render::Init>
        (
            SDL_GL_GetProcAddress,
            blend_hack
                ? render::Init::BlendHack::enable_hack
               : render::Init::BlendHack::no_hack
        );

        if(init->is_ok == false)
        {
            LOG_ERROR("Failed to create Init");
            return false;
        }

        render::setup_opengl_debug();

        imgui = std::make_unique<imgui::Library>(window->window, context.get(), pref_path);
        ImGui::StyleColorsLight();

        return true;
    }


    bool
    Engine::on_resize(SDL_Event e, int* width, int* height) const
    {
        if(e.type == SDL_WINDOWEVENT)
        {
            if(e.window.windowID == window_id)
            {
                switch(e.window.event)
                {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    SDL_GetWindowSize(window->window, width, height);
                    return true;
                default:
                    break;
                }
            }
        }

        return false;
    }

}
