#pragma once

#include "core/noncopyable.h"


namespace euphoria::gui
{
    struct button;
    struct panel_widget;
}


namespace euphoria::gui
{
    struct visitor
    {
        visitor() = default;
        virtual ~visitor() = default;
        NONCOPYABLE(visitor);

        virtual
        void
        visit(button* button) = 0;

        virtual
        void
        visit(panel_widget* button) = 0;
    };
}

