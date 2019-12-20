#ifndef EUPHORIA_T3D_EDITOR_H
#define EUPHORIA_T3D_EDITOR_H

#include "core/key.h"
#include "core/vec2.h"
#include "core/camera.h"

#include "render/viewport.h"

#include "t3d/toolstack.h"


namespace euphoria::render
{
    struct World;
    struct Actor;
    struct CompiledMesh;
}


namespace euphoria::t3d
{
    struct TileLibrary;
    struct Grid;


    // todo(Gustav): Should editor and T3d structs be merged?
    struct Editor
    {
        Grid* grid;
        render::World*         world;
        TileLibrary*   tile_library;
        core::CompiledCamera camera;
        render::Viewport       viewport;

        core::vec2i mouse;
        Tools tools;

        std::vector<std::shared_ptr<render::Actor>> actors;
        std::shared_ptr<render::CompiledMesh>       selected_mesh;

        // todo(Gustav): move camera here so we can have camera movement
        // change so that fps control rotate focuspoint around current camera pos


        Editor(Grid* agrid, render::World* aworld, TileLibrary* atile_library);


        // current tool callbacks
        void
        MeshHasChanged();


        bool
        IsBusy();


        void
        Step();


        void
        OnMouse(core::MouseButton button, bool down);


        void
        OnKey(core::Key key, bool down);


        void
        OnScroll(const core::vec2i& scroll);


        void
        OnEditor();
    };
}

#endif  // EUPHORIA_T3D_EDITOR_H
