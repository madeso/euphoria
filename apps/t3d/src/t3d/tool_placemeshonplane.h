#pragma once

#include <memory>

#include "base/plane.h"

#include "t3d/tool.h"


namespace eu::render
{
    struct Actor;
}


namespace eu::t3d
{
    struct PlacedMesh;

    struct ToolPlaceMeshOnAPlane : public Tool
    {
        std::shared_ptr<PlacedMesh> placed_mesh;
        Plane plane;


        ToolPlaceMeshOnAPlane(std::shared_ptr<PlacedMesh> aactor);


        bool
        is_busy(Editor*) override;


        void
        update(Editor* editor) override;


        void
        on_mouse(Editor* editor, core::MouseButton button, const KeyboardState& state, bool down) override;


        void
        on_key(Editor* editor, core::Key key, bool down) override;


        void
        on_scroll(Editor*, const vec2i&) override;


        void
        on_editor(Editor*) override;
    };
}
