#pragma once

#include "core/key.h"
#include "core/vec2.h"
#include "core/camera3.h"

#include "render/viewport.h"

#include "t3d/toolstack.h"


namespace euphoria::core
{
    struct UnitRay3f;
}


namespace euphoria::render
{
    struct World;
    struct Actor;
}


namespace euphoria::t3d
{
    struct Tile;
    struct TileLibrary;
    struct Grid;


    struct PlacedMesh
    {
        std::shared_ptr<render::Actor> actor;
        std::shared_ptr<t3d::Tile> tile;
        bool is_selected = false;
    };


    // todo(Gustav): Should editor and T3d structs be merged?
    struct Editor
    {
        t3d::Grid* grid;
        render::World* world;
        t3d::TileLibrary* tile_library;
        core::CompiledCamera3 camera;
        render::Viewport viewport;

        core::Vec2i mouse;
        ToolStack tools;

        std::vector<std::shared_ptr<PlacedMesh>> placed_meshes;
        std::shared_ptr<Tile> selected_tile;

        // todo(Gustav): move camera here so we can have camera movement
        // change so that fps control rotate focuspoint around current camera pos


        Editor(t3d::Grid* agrid, render::World* aworld, t3d::TileLibrary* atile_library);


        std::shared_ptr<PlacedMesh>
        get_first_selected_or_null();


        void
        set_all_selected(bool is_selected);


        std::vector<std::shared_ptr<PlacedMesh>>
        raycast(const core::UnitRay3f& ray);


        // current tool callbacks
        bool
        is_busy();


        void
        step();


        void
        on_mouse(core::MouseButton button, bool down);


        void
        on_key(core::Key key, bool down);


        void
        on_scroll(const core::Vec2i& scroll);


        void
        on_editor();

        std::vector<int>
        get_selected_indices() const;

        void
        run_tools(bool show_imgui, const core::CompiledCamera3& cc);
    };
}
