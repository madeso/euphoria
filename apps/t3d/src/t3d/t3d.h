#pragma once

#include <memory>
#include <optional>

#include "imgui/imgui.h"

#include "core/rgb.h"
#include "core/key.h"
#include "base/vec2.h"
#include "core/camera3.h"
#include "core/camera3.editor.h"
#include "io/vfs_path.h"
#include "core/mousebehaviour.h"
#include "core/helptexthover.h"

#include "render/viewporthandler.h"

#include "window/fakesdlevent.h"

#include "t3d/grid.h"
#include "t3d/filelist.h"
#include "t3d/keyboardstate.h"


namespace eu::core
{
    struct Lines;
    struct UnitRay3f;
}

namespace eu::core::argparse
{
    struct NameAndArguments;
}

namespace eu::render
{
    struct MaterialShaderCache;
    struct TextureCache;
    struct World;
    struct PositionedLines;
}

namespace eu::window
{
    struct Engine;
    struct SdlTimer;
}


namespace eu::t3d
{
    struct Editor;
    struct TileLibrary;
    struct Application;

    struct EditorCamera3 : public core::EditorCamera3
    {
        Application* parent = nullptr;

        std::optional<vec3f>
        raycast
        (
            const core::UnitRay3f& ray
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

        core::HelpTextHover help_hover;
        int help_style = 3;
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

        Application(const Application& other) = delete;
        void operator=(const Application&) = delete;
        Application(Application&& other) = delete;
        void operator=(Application&&) = delete;


        [[nodiscard]] int
        start(const core::argparse::NameAndArguments& args);


        void
        add_library(const io::DirPath& path);

        void
        update_grid();


        [[nodiscard]] KeyboardState
        calculate_keyboard_state() const;


        void
        on_sdl_event(const SDL_Event& e);


        void
        focus_selected();


        void
        on_mouse_movement(const vec2i& position, const vec2i& movement, bool forward_mouse);


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
