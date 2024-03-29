#pragma once

#include "base/rect.h"
#include "base/size2.h"
#include "base/lrud.h"

#include "gui/layoutdata.h"


namespace eu::render
{
    struct SpriteRenderer;
}

namespace eu::gui
{
    struct State;
    struct Visitor;
}

namespace eu::gui
{
    struct Widget
    {
        std::string name;
        Lrud<float> margin;
        Lrud<float> padding;

        explicit
        Widget(gui::State* state);

        virtual
        ~Widget();

        Widget(const Widget& other) = delete;
        void operator=(const Widget&) = delete;
        Widget(Widget&& other) = delete;
        void operator=(Widget&&) = delete;

        [[nodiscard]] bool
        is_active() const;

        [[nodiscard]] bool
        is_hot() const;

        virtual
        void
        update(float dt) = 0;

        virtual
        void
        on_size_changed();

        [[nodiscard]] Rectf
        get_client_rect() const;

        [[nodiscard]] Rectf
        get_background_rect() const;

        void
        set_rect(const Rectf& r);

        [[nodiscard]] size2f
        get_preferred_size() const;

        [[nodiscard]] virtual
        size2f
        calc_minimum_size() const = 0;

        virtual
        void
        render(render::SpriteRenderer* renderer) const = 0;

        virtual
        void
        visit(Visitor* visitor) = 0;

        [[nodiscard]] const gui::State&
        get_state() const;

        [[nodiscard]] gui::State*
        get_state_ptr() const;

        gui::State* ui_state;

        LayoutData layout;
        Rectf rect;
    };
}
