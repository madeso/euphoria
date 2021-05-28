#pragma once

#include "core/key.h"
#include "core/vec2.h"
#include "core/camera3.h"

#include "render/viewport.h"

#include "t3d/toolstack.h"


namespace euphoria::core
{
    struct unit_ray3f;
}


namespace euphoria::render
{
    struct world;
    struct actor;
}


namespace euphoria::t3d
{
    struct tile;
    struct tile_library;
    struct grid;


    struct placed_mesh
    {
        std::shared_ptr<render::actor> actor;
        std::shared_ptr<t3d::tile> tile;
        bool is_selected = false;
    };


    // todo(Gustav): Should editor and T3d structs be merged?
    struct editor
    {
        t3d::grid* grid;
        render::world* world;
        t3d::tile_library* tile_library;
        core::compiled_camera3 camera;
        render::viewport viewport;

        core::vec2i mouse;
        tool_stack tools;

        std::vector<std::shared_ptr<placed_mesh>> actors;
        std::shared_ptr<tile> selected_mesh;

        // todo(Gustav): move camera here so we can have camera movement
        // change so that fps control rotate focuspoint around current camera pos


        editor(t3d::grid* agrid, render::world* aworld, t3d::tile_library* atile_library);


        std::shared_ptr<placed_mesh>
        get_first_selected_or_null();


        void
        set_all_selected(bool is_selected);


        std::vector<std::shared_ptr<placed_mesh>>
        raycast(const core::unit_ray3f& ray);


        // current tool callbacks
        bool
        is_busy();


        void
        step();


        void
        on_mouse(core::MouseButton button, bool down);


        void
        on_key(core::key key, bool down);


        void
        on_scroll(const core::vec2i& scroll);


        void
        on_editor();
    };
}
