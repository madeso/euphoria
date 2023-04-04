#pragma once

#include <memory>

#include "core/argparse.h"

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
    struct SdlOpenglContext;

    struct Engine
    {
        std::unique_ptr<SdlLibrary> sdl;
        std::unique_ptr<core::vfs::FileSystem> file_system;
        std::shared_ptr<core::vfs::ReadRootCatalog> catalog;
        std::unique_ptr<SdlWindow> window;
        unsigned int window_id;
        std::unique_ptr<SdlOpenglContext> context;
        std::unique_ptr<render::Init> init;
        std::unique_ptr<imgui::Library> imgui;

        Engine();
        ~Engine();

        Engine(const Engine&) = delete;
        Engine(Engine&&) = delete;
        void operator=(const Engine&) = delete;
        void operator=(Engine&&) = delete;

        int setup(const core::argparse::NameAndArguments& args);

        bool create_window
        (
            const std::string& title,
            int width,
            int height,
            bool blend_hack = false
        );

        bool on_resize(SDL_Event e, int* width, int* height) const;
    };
}
