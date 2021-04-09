#ifndef GUI_UISTATE_H
#define GUI_UISTATE_H

#include "core/vec2.h"

namespace euphoria::gui
{
    struct Widget;
}

namespace euphoria::gui
{
    struct UiState
    {
    public:
        UiState() = default;

        core::vec2f mouse = core::vec2f(0.0f, 0.0f);
        bool mouse_down = false;

        Widget* hot = nullptr;
        Widget* active = nullptr;
        bool has_active = false;

        void
        Begin();

        void
        End();

        void
        SetHot(Widget* w);

        void
        SetActive(Widget* w);

        [[nodiscard]] bool
        IsMouseDown() const;
    };
}

#endif  // GUI_UISTATE_H

