#pragma once

#include <string>

#include "core/rect.h"
#include "core/size.h"
#include "core/noncopyable.h"

#include "gui/layoutdata.h"


namespace euphoria::render
{
    struct sprite_renderer;
}

namespace euphoria::gui
{
    struct UiState;
    struct Visitor;
}

namespace euphoria::gui
{
    struct Lrtb
    {
        float left = 0.0f;
        float right = 0.0f;
        float top = 0.0f;
        float bottom = 0.0f;
    };


    struct Widget
    {
        std::string name;
        Lrtb margin;
        Lrtb padding;

        explicit
        Widget(UiState* state);

        virtual
        ~Widget();

        NONCOPYABLE(Widget);

        [[nodiscard]] bool
        IsActive() const;

        [[nodiscard]] bool
        IsHot() const;

        virtual
        void
        Step(float dt) = 0;

        virtual
        void
        OnSize();

        [[nodiscard]] core::rectf
        GetClientRect() const;

        [[nodiscard]] core::rectf
        GetBackgroundRect() const;

        void
        SetRect(const core::rectf& r);

        [[nodiscard]] core::Sizef
        GetPreferredSize() const;

        [[nodiscard]] virtual
        core::Sizef
        CalculateMinimumSize() const = 0;

        virtual
        void
        Render(render::sprite_renderer* renderer) const = 0;

        virtual
        void
        Visit(Visitor* visitor) = 0;

        [[nodiscard]] const UiState&
        GetState() const;

        [[nodiscard]] UiState*
        GetStatePtr() const;

        UiState* state_;

        LayoutData layout;
        core::rectf rect_;
    };
}
