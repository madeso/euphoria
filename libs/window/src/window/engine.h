#pragma once

#include <memory>
#include <string>

#include "core/argparse.h"

#include "window/fakesdlevent.h"


namespace euphoria::core::vfs
{
    struct file_system;
    struct read_root_catalog;
}

namespace euphoria::render
{
    struct init;
}

namespace euphoria::window::imgui
{
    struct library;
}

namespace euphoria::window
{
    struct sdl_library;
    struct sdl_window;
    struct sdl_gl_context;

    struct engine
    {
        engine();
        ~engine();

        int
        setup(const core::argparse::name_and_arguments& args);

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

        std::unique_ptr<sdl_library> sdl;
        std::unique_ptr<core::vfs::file_system> file_system;
        std::shared_ptr<core::vfs::read_root_catalog> catalog;
        std::unique_ptr<sdl_window> window;
        unsigned int window_id;
        std::unique_ptr<sdl_gl_context> context;
        std::unique_ptr<render::init> init;
        std::unique_ptr<imgui::library> imgui;
    };
}
