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
    struct Engine;
    struct SdlTimer;
}


namespace euphoria::t3d
{
    struct Editor;
    struct TileLibrary;

    struct T3d
    {
        std::shared_ptr<window::Engine> engine;
        std::shared_ptr<render::viewport_handler> viewport_handler;
        std::shared_ptr<render::material_shader_cache> material_shader_cache;
        std::shared_ptr<render::texture_cache> texture_cache;
        std::shared_ptr<TileLibrary> tile_library;
        std::shared_ptr<render::world> world;
        core::camera3 camera;
        std::shared_ptr<Editor> editor;
        bool running = true;
        std::shared_ptr<window::SdlTimer> timer;
        bool immersive_mode = false;
        std::shared_ptr<render::positioned_lines> grid;
        Grid grid_data;

        bool enviroment_window = false;
        bool camera_window     = false;
        bool tiles_window      = true;
        bool grid_window      = true;
        bool lister_window = true;
        bool preference_window = false;

        core::orbit_controller orbit;
        bool mmb_down = false;
        bool shift_down = false;
        bool show_imgui = false;

        int help_style = 0;
        FileList pending_files;


        T3d();
        ~T3d();


        [[nodiscard]] int
        Start(const core::argparse::name_and_arguments& args);


        void
        AddLibrary(const core::vfs::dir_path& path);


        void
        AddSingleGridLine
        (
            core::lines& def,
            float size,
            float x,
            const core::rgb& color
        );


        void
        UpdateGrid();


        void
        OnSingleEvent(const SDL_Event& e);


        void
        OnMouseMovement(const core::vec2i& position, const core::vec2i& movement, bool forward_mouse);


        void
        OnKey(core::key key, bool down, bool forward_keyboard);


        void
        OnMouseButton(core::MouseButton button, bool down, bool forward_mouse);


        void
        OnMouseWheel(const SDL_Event& e, bool forward_mouse);


        void
        HandleEvents();


        void
        ProcessImgui();


        void
        OnMainMenu();


        void
        OnListerWindow();


        void
        OnEnviromentWindow();


        void
        OnGridWindow();


        void
        OnCameraWindow();


        void
        OnTileWindow();


        void
        OnPreferenceWindow();


        void
        Render();


        void
        Frame();

        void
        Help(const char* desc);
    };
}

#endif  // EUPHORIA_T3D_T3D_H
