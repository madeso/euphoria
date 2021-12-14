#pragma once

#include <memory>
#include <string>

#include "core/argparse.h"
#include "core/noncopyable.h"

#include "window/fakesdlevent.h"


namespace euphoria::core::vfs
{
    struct FileSystem;
    struct ReadRootCatalog;
}

namespace euphoria::render
{
    struct Init;
}

namespace euphoria::window::imgui
{
    struct Library;
}

namespace euphoria::window
{
    struct SdlLibrary;
    struct SdlWindow;
    struct SdlAndOpenglContext;

    struct Engine
    {
        Engine();
        ~Engine();

        NONCOPYABLE(Engine);

        int
        setup(const core::argparse::NameAndArguments& args);

        bool
        create_window
        (
            const std::string& title,
            int width,
            int height,
            bool blend_hack = false
        );

        bool
        on_resize(SDL_Event e, int* width, int* height) const;

        std::unique_ptr<SdlLibrary> sdl;
        std::unique_ptr<core::vfs::FileSystem> file_system;
        std::shared_ptr<core::vfs::ReadRootCatalog> catalog;
        std::unique_ptr<SdlWindow> window;
        unsigned int window_id;
        std::unique_ptr<SdlAndOpenglContext> context;
        std::unique_ptr<render::Init> init;
        std::unique_ptr<imgui::Library> imgui;
    };
}
