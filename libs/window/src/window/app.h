#pragma once


#include <functional>
#include <memory>

// for sdl main to work
#include "SDL.h"

#include "base/rgb.h"


namespace eu::window
{
    struct App
    {
        bool running = true;
        Rgb clear_color;

        App();
        virtual ~App() = default;

        App(const App&) = delete;
        App(App&&) = delete;
        void operator=(const App&) = delete;
        void operator=(App&&) = delete;

        virtual void on_quit();
        virtual void on_gui();
    };


    using CreateAppFunction = std::function
    <
        std::unique_ptr<App>
        ()
    >;


    int run_app(int argc, char** argv, const std::string& title, CreateAppFunction create);
}
