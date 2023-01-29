#pragma once


#include <functional>
#include <memory>

// for sdl main to work
#include "SDL.h"

#include "core/rgb.h"


namespace euphoria::window
{
    struct App
    {
        App();

        App(const App&) = delete;
        App(App&&) = delete;

        void operator=(const App&) = delete;
        void operator=(App&&) = delete;

        virtual ~App() = default;

        virtual void on_quit();
        virtual void on_gui();

        bool running = true;
        core::rgb clear_color;
    };


    using CreateAppFunction = std::function
    <
        std::unique_ptr<App>
        ()
    >;


    int run_app(int argc, char** argv, const std::string& title, CreateAppFunction create);
}
