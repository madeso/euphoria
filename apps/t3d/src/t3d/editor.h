#ifndef EUPHORIA_T3D_EDITOR_H
#define EUPHORIA_T3D_EDITOR_H

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
    struct Tile;
    struct TileLibrary;
    struct Grid;


    struct PlacedMesh
    {
        std::shared_ptr<render::actor> actor;
        std::shared_ptr<Tile> tile;
        bool is_selected = false;
    };


    // todo(Gustav): Should editor and T3d structs be merged?
    struct Editor
    {
        Grid* grid;
        render::world*         world;
        TileLibrary*   tile_library;
        core::compiled_camera3 camera;
        render::viewport       viewport;

        core::vec2i mouse;
        Tools tools;

        std::vector<std::shared_ptr<PlacedMesh>> actors;
        std::shared_ptr<Tile> selected_mesh;

        // todo(Gustav): move camera here so we can have camera movement
        // change so that fps control rotate focuspoint around current camera pos


        Editor(Grid* agrid, render::world* aworld, TileLibrary* atile_library);


        std::shared_ptr<PlacedMesh>
        GetFirstSelectedOrNull();


        void
        SetAllSelected(bool is_selected);


        std::vector<std::shared_ptr<PlacedMesh>>
        Raycast(const core::unit_ray3f& ray);


        // current tool callbacks
        bool
        IsBusy();


        void
        Step();


        void
        OnMouse(core::MouseButton button, bool down);


        void
        OnKey(core::key key, bool down);


        void
        OnScroll(const core::vec2i& scroll);


        void
        OnEditor();
    };
}

#endif  // EUPHORIA_T3D_EDITOR_H
