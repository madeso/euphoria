#ifndef EUPHORIA_ENGINE_H
#define EUPHORIA_ENGINE_H

#include <memory>
#include <string>

#include "core/argparse.h"

#include "window/fakesdlevent.h"


namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FileSystemRootCatalog;
    }  // namespace vfs
}  // namespace euphoria::core

namespace euphoria::render
{
    struct Init;
}

namespace euphoria::window
{
    struct SdlLibrary;
    struct SdlWindow;
    struct ImguiLibrary;
    struct SdlGlContext;
    struct ImguiLibrary;

    struct Engine
    {
        Engine();
        ~Engine();

        int
        Setup(const core::argparse::Arguments& args);

        bool
        CreateWindow(
                const std::string& title,
                int                width,
                int                height,
                bool               blend_hack = false);

        bool
        HandleResize(SDL_Event e, int* width, int* height);

        std::unique_ptr<SdlLibrary>                       sdl;
        std::unique_ptr<core::vfs::FileSystem>            file_system;
        std::shared_ptr<core::vfs::FileSystemRootCatalog> catalog;
        std::unique_ptr<SdlWindow>                        window;
        unsigned int                                      window_id;
        std::unique_ptr<SdlGlContext>                     context;
        std::unique_ptr<render::Init>                     init;
        std::unique_ptr<ImguiLibrary>                     imgui;
    };
}  // namespace euphoria::window


#endif  // EUPHORIA_ENGINE_H
