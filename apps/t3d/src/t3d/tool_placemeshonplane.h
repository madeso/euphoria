#ifndef EUPHORIA_T3D_TOOL_PLACEMESHONPLANE_H
#define EUPHORIA_T3D_TOOL_PLACEMESHONPLANE_H

#include <memory>

#include "core/plane.h"

#include "t3d/tool.h"


namespace euphoria::render
{
    struct Actor;
}


namespace euphoria::t3d
{
    struct PlacedMesh;

    struct PlaceMeshOnPlane : public Tool
    {
        std::shared_ptr<PlacedMesh> actor;
        core::plane plane;


        PlaceMeshOnPlane(std::shared_ptr<PlacedMesh> aactor);


        bool
        IsBusy(Editor*) override;


        void
        Step(Editor* editor) override;


        void
        OnMouse(Editor* editor, core::MouseButton button, bool down) override;


        void
        OnKey(Editor* editor, core::key key, bool down) override;


        void
        OnScroll(Editor*, const core::vec2i&) override;


        void
        OnEditor(Editor*) override;
    };
}

#endif  // EUPHORIA_T3D_TOOL_PLACEMESHONPLANE_H
