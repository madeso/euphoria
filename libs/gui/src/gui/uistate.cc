#include "gui/uistate.h"


namespace eu::gui
{
    void
    State::begin()
    {
        hot = nullptr;
    }


    void
    State::end()
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
    State::set_hot(Widget* w)
    {
        hot = w;
    }


    void
    State::set_active(Widget* w)
    {
        if(has_active == false && active == nullptr)
        {
            active = w;
            has_active = true;
        }
    }


    bool
    State::is_mouse_down() const
    {
        return mouse_down;
    }
}

