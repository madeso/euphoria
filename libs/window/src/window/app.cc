#include "window/app.h"

#include "imgui/imgui.h"
#include "SDL.h"

#include "log/log.h"

#include "core/argparse.h"
#include "core/colors.h"

#include "render/init.h"

#include "window/engine.h"
#include "window/imgui_extra.h"
#include "window/imguilibrary.h"
#include "window/sdlwindow.h"

namespace eu::window
{
    App::App()
        : clear_color(core::NamedColor::light_gray)
    {
    }

    void App::on_quit()
    {
        running = false;
    }


    void App::on_gui()
    {
        ImGui::ShowDemoWindow();
    }


    int
    run_app(int argc, char** argv, const std::string& window_title, CreateAppFunction create_app)
    {
        EU_INIT_LOGGING();
        Engine engine;

        if (const auto r = engine.setup(core::argparse::NameAndArguments::extract(argc, argv)); r != 0)
        {
            return r;
        }


        int window_width = 1280;
        int window_height = 720;

        if(!engine.create_window(window_title, window_width, window_height, true))
        {
            return -1;
        }

        //////////////////////////////////////////////////////////////////////////////
        // main loop
        auto app = create_app();

        while(app->running)
        {
            SDL_Event e;
            while(SDL_PollEvent(&e) != 0)
            {
                imgui::send_events_to_imgui(&e);

                if(engine.on_resize(e, &window_width, &window_height))
                {
                    // viewport_handler.set_size(window_width, window_height);
                }

                switch(e.type)
                {
                case SDL_QUIT:
                    app->on_quit();
                    break;
                default:
                    // ignore other events
                    break;
                }
            }

            imgui::begin_new_frame();
            
            app->on_gui();

            engine.init->clear_screen(app->clear_color);
            imgui::imgui_render();

            SDL_GL_SwapWindow(engine.window->window);
        }

        return 0;
    }
}
