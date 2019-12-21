#ifndef EUPHORIA_T3D_T3D_H
#define EUPHORIA_T3D_T3D_H


#include <string>
#include <memory>

#include "core/rgb.h"
#include "core/key.h"
#include "core/vec2.h"
#include "core/camera.h"
#include "core/orbitcontroller.h"

#include "render/viewporthandler.h"

#include "window/fakesdlevent.h"

#include "t3d/grid.h"


namespace euphoria::core
{
    struct Lines;
}

namespace euphoria::core::argparse
{
    struct Args;
}

namespace euphoria::render
{
    struct MaterialShaderCache;
    struct TextureCache;
    struct World;
    struct PositionedLines;
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
        render::ViewportHandler viewport_handler;
        std::shared_ptr<render::MaterialShaderCache> material_shader_cache;
        std::shared_ptr<render::TextureCache> texture_cache;
        std::shared_ptr<TileLibrary> tile_library;
        std::shared_ptr<render::World> world;
        core::Camera camera;
        std::shared_ptr<Editor> editor;
        bool running = true;
        std::shared_ptr<window::SdlTimer> timer;
        bool immersive_mode = false;
        std::shared_ptr<render::PositionedLines> grid;
        Grid grid_data;

        bool enviroment_window = false;
        bool camera_window     = false;
        bool tiles_window      = true;
        bool grid_window      = true;
        bool lister_window = true;

        core::OrbitController orbit;
        bool mmb_down = false;
        bool shift_down = false;
        bool show_imgui = false;


        T3d() = default;
        ~T3d();


        [[nodiscard]] bool
        Start(const core::argparse::Args& args);


        void
        AddLibrary(const std::string& path);


        void
        AddSingleGridLine
        (
            core::Lines& def,
            float size,
            float x,
            const core::Rgb& color
        );


        void
        UpdateGrid();


        void
        OnSingleEvent(const SDL_Event& e);


        void
        OnMouseMovement(const core::vec2i& position, const core::vec2i& movement, bool forward_mouse);


        void
        OnKey(core::Key key, bool down, bool forward_keyboard);


        void
        OnMouseButton(core::MouseButton button, bool down, bool forward_mouse);


        void
        OnMouseWheel(const SDL_Event& e, bool forward_mouse);


        void
        HandleEvents();

        void
        SetAllSelected(bool is_selected);
        

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
        Render();


        void
        Frame();
    };
}

#endif  // EUPHORIA_T3D_T3D_H
