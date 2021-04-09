#include "window/engine.h"

#include "SDL.h"
#include "imgui/imgui.h"

#include "core/os.h"
#include "core/log.h"
#include "core/vfs.h"
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

namespace euphoria::window
{
    LOG_SPECIFY_DEFAULT_LOGGER("window.engine")


    Engine::Engine() = default;


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
    Engine::Setup(const core::argparse::Arguments& args)
    {
        sdl = std::make_unique<SdlLibrary>();
        if(sdl->ok == false)
        {
            LOG_ERROR("Failed to create SDL");
            return false;
        }

        auto parser = core::argparse::Parser("euphoria engine");

        auto current_directory = core::GetCurrentDirectory();
        parser.Add("-w", &current_directory)
                .Help("Sets the working direction if it's differnt from the current folder");
        const auto parse_result = parser.Parse(args);
        if(parse_result != core::argparse::ParseResult::Ok)
        {
            return parse_result.return_value;
        }

        LOG_INFO("Current directory: {0}", current_directory);

        file_system.reset(new core::vfs::FileSystem {});
        catalog = core::vfs::FileSystemRootCatalog::AddRoot(file_system.get());
        core::vfs::FileSystemRootFolder::AddRoot
        (
            file_system.get(),
            current_directory
        );
        core::vfs::FileSystemImageGenerator::AddRoot
        (
            file_system.get(),
            core::vfs::DirPath{"~/img-plain/"}
        );
        core::vfs::AddDefaultShaders
        (
            file_system.get(),
            core::vfs::DirPath{"~/shaders/"}
        );

        render::SetupDefaultFiles(catalog);

        return 0;
    }


    bool
    Engine::CreateWindow
    (
        const std::string& title,
        int width,
        int height,
        bool blend_hack
    )
    {
        const auto pref_path = GetPrefPath();

        window = std::make_unique<SdlWindow>(title, width, height, true);

        if(window->window == nullptr)
        {
            LOG_ERROR("Failed to create window {0}", SDL_GetError());
            return false;
        }

        window_id = SDL_GetWindowID(window->window);

        context.reset(new SdlGlContext {window.get()});

        if(context->context == nullptr)
        {
            LOG_ERROR("Failed to create SDL context {0}", SDL_GetError());
            return false;
        }

        init.reset(new render::Init {
                SDL_GL_GetProcAddress,
                blend_hack ? render::Init::BlendHack::EnableHack
                           : render::Init::BlendHack::NoHack});

        if(init->ok == false)
        {
            LOG_ERROR("Failed to create Init");
            return false;
        }

        render::SetupOpenglDebug();

        imgui.reset(new ImguiLibrary {window->window, context.get(), pref_path});
        ImGui::StyleColorsLight();

        return true;
    }


    bool
    Engine::HandleResize(SDL_Event e, int* width, int* height)
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
                default: break;
                }
            }
        }

        return false;
    }

}  // namespace euphoria::window
