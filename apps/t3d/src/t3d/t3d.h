#pragma once

#include "core/rgb.h"
#include "core/key.h"
#include "core/vec2.h"
#include "core/camera3.h"
#include "core/camera3.editor.h"
#include "core/vfs_path.h"
#include "core/noncopyable.h"
#include "core/mousebehaviour.h"

#include "render/viewporthandler.h"

#include "window/fakesdlevent.h"

#include "t3d/grid.h"
#include "t3d/filelist.h"
#include "t3d/keyboardstate.h"

#include <vector>
#include <string>
#include <memory>
#include <optional>


namespace euphoria::core
{
    struct Lines;
}

namespace euphoria::core::argparse
{
    struct NameAndArguments;
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
    struct Application;

    struct EditorCamera3 : public core::EditorCamera3
    {
        Application* parent = nullptr;

        std::optional<core::Vec3f>
        raycast
        (
            const core::Vec2i& mouse,
            const core::CompiledCamera3& camera,
            const core::Viewport& viewport
        ) override;
    };

    struct Application
    {
        std::shared_ptr<window::Engine> engine;
        std::shared_ptr<render::ViewportHandler> viewport_handler;
        std::shared_ptr<render::MaterialShaderCache> material_shader_cache;
        std::shared_ptr<render::TextureCache> texture_cache;
        std::shared_ptr<t3d::TileLibrary> tile_library;
        std::shared_ptr<render::World> world;
        core::Camera3 camera;
        std::optional<core::CompiledCamera3> compiled_camera;
        std::shared_ptr<t3d::Editor> editor;
        bool running = true;
        std::shared_ptr<window::SdlTimer> timer;
        bool immersive_mode = false;
        std::shared_ptr<render::PositionedLines> grid;
        t3d::Grid grid_data;

        bool environment_window = false;
        bool camera_window = true;
        bool tiles_window = true;
        bool grid_window = true;
        bool lister_window = true;
        bool preference_window = false;

        EditorCamera3 editor_camera;
        bool mmb_down = false;
        bool shift_down = false;
        bool show_gizmo = true;

        int help_style = 0;
        FileList pending_files;

        // current keyboard state
        bool state_shift_left = false;
        bool state_shift_right = false;
        bool state_ctrl_left = false;
        bool state_ctrl_right = false;
        bool state_alt_left = false;
        bool state_alt_right = false;

        std::optional<core::MouseBehaviour> current_mouse;
        void
        update_mouse();


        Application();
        ~Application();

        NONCOPYABLE(Application);


        [[nodiscard]] int
        start(const core::argparse::NameAndArguments& args);


        void
        add_library(const core::vfs::DirPath& path);

        void
        update_grid();


        [[nodiscard]] KeyboardState
        calculate_keyboard_state() const;


        void
        on_sdl_event(const SDL_Event& e);


        void
        on_mouse_movement(const core::Vec2i& position, const core::Vec2i& movement, bool forward_mouse);


        void
        on_key(core::Key key, bool down, bool forward_keyboard);


        void
        on_mouse_button(core::MouseButton button, bool down, bool forward_mouse);


        void
        on_mouse_wheel(const SDL_Event& e, bool forward_mouse);


        void
        handle_all_sdl_events();


        void
        process_imgui(bool display_full);


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
        guizmo
        (
            bool is_translate, bool global_space,
            bool translate_x, bool translate_y, bool translate_z,
            bool rotate_x, bool rotate_y, bool rotate_z
        );

        void
        show_help(const char* desc) const;
    };
}
