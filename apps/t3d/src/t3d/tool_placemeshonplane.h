#pragma once

#include <memory>

#include "core/plane.h"

#include "t3d/tool.h"


namespace euphoria::render
{
    struct actor;
}


namespace euphoria::t3d
{
    struct placed_mesh;

    struct tool_place_mesh_on_a_plane : public tool
    {
        std::shared_ptr<placed_mesh> actor;
        core::plane plane;


        tool_place_mesh_on_a_plane(std::shared_ptr<placed_mesh> aactor);


        bool
        is_busy(editor*) override;


        void
        step(editor* editor) override;


        void
        on_mouse(editor* editor, core::mouse_button button, bool down) override;


        void
        on_key(editor* editor, core::key key, bool down) override;


        void
        on_scroll(editor*, const core::vec2i&) override;


        void
        on_editor(editor*) override;
    };
}
