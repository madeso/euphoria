#ifndef EUPHORIA_T3D_T3D_H
#define EUPHORIA_T3D_T3D_H


#include <string>
#include <memory>

#include "core/rgb.h"
#include "core/key.h"
#include "core/vec2.h"
#include "core/camera3.h"
#include "core/orbitcontroller.h"
#include "core/vfs_path.h"

#include "render/viewporthandler.h"

#include "window/fakesdlevent.h"

#include "t3d/grid.h"
#include "t3d/filelist.h"


namespace euphoria::core
{
    struct lines;
}

namespace euphoria::core::argparse
{
    struct name_and_arguments;
}

namespace euphoria::render
{
    struct material_shader_cache;
    struct texture_cache;
    struct world;
    struct positioned_lines;
}

namespace euphoria::window
{
    struct engine;
    struct sdl_timer;
}


namespace euphoria::t3d
{
    struct editor;
    struct tile_library;

    struct application
    {
        std::shared_ptr<window::engine> engine;
        std::shared_ptr<render::viewport_handler> viewport_handler;
        std::shared_ptr<render::material_shader_cache> material_shader_cache;
        std::shared_ptr<render::texture_cache> texture_cache;
        std::shared_ptr<tile_library> tile_library;
        std::shared_ptr<render::world> world;
        core::camera3 camera;
        std::shared_ptr<t3d::editor> editor;
        bool running = true;
        std::shared_ptr<window::sdl_timer> timer;
        bool immersive_mode = false;
        std::shared_ptr<render::positioned_lines> grid;
        t3d::grid grid_data;

        bool environment_window = false;
        bool camera_window = false;
        bool tiles_window = true;
        bool grid_window = true;
        bool lister_window = true;
        bool preference_window = false;

        core::orbit_controller orbit;
        bool mmb_down = false;
        bool shift_down = false;
        bool show_imgui = false;

        int help_style = 0;
        file_list pending_files;


        application();
        ~application();


        [[nodiscard]] int
        start(const core::argparse::name_and_arguments& args);


        void
        add_library(const core::vfs::dir_path& path);


        void
        add_single_grid_line
        (
            core::lines& def,
            float size,
            float x,
            const core::rgb& color
        );


        void
        update_grid();


        void
        on_sdl_event(const SDL_Event& e);


        void
        on_mouse_movement(const core::vec2i& position, const core::vec2i& movement, bool forward_mouse);


        void
        on_key(core::key key, bool down, bool forward_keyboard);


        void
        on_mouse_button(core::MouseButton button, bool down, bool forward_mouse);


        void
        on_mouse_wheel(const SDL_Event& e, bool forward_mouse);


        void
        handle_all_sdl_events();


        void
        process_imgui();


        void
        on_main_menu();


        void
        on_lister_window();


        void
        on_environment_window();


        void
        on_grid_window();


        void
        on_camera_window();


        void
        on_tile_window();


        void
        on_preference_window();


        void
        render();


        void
        on_frame();

        void
        show_help(const char* desc);
    };
}

#endif // EUPHORIA_T3D_T3D_H
