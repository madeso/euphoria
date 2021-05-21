#include "gui/uistate.h"


namespace euphoria::gui
{
    void
    ui_state::begin()
    {
        hot = nullptr;
    }


    void
    ui_state::end()
    {
        if(is_mouse_down() == false)
        {
            active = nullptr;
            has_active = false;
        }
        else
        {
            if(has_active == false && active == nullptr)
            {
                has_active = true;
            }
        }
    }


    void
    ui_state::set_hot(widget* w)
    {
        hot = w;
    }


    void
    ui_state::set_active(widget* w)
    {
        if(has_active == false && active == nullptr)
        {
            active = w;
            has_active = true;
        }
    }


    bool
    ui_state::is_mouse_down() const
    {
        return mouse_down;
    }
}

