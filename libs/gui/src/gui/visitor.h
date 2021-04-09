#pragma once

#include "core/noncopyable.h"


namespace euphoria::gui
{
    struct Button;
    struct PanelWidget;
}


namespace euphoria::gui
{
    struct Visitor
    {
        Visitor() = default;
        virtual ~Visitor() = default;
        NONCOPYABLE(Visitor);

        virtual
        void
        Visit(Button* button) = 0;

        virtual
        void
        Visit(PanelWidget* button) = 0;
    };
}

