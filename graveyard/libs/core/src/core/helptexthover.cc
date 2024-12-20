#include "core/helptexthover.h"

#include "assert/assert.h"


namespace
{
    enum State : int
    {
        state_no_hover,
        state_hover_text
    };

    // return if the help text should be displayed or not
    bool
    update
    (
        int& state,
        float& timer,
        float time_to_hover,
        float time_to_hide,
        bool currently_hovering_over_widget, float dt
    )
    {
        switch(state)
        {
        case state_no_hover:
            if(currently_hovering_over_widget)
            {
                timer += dt;

                if(timer > time_to_hover)
                {
                    timer = 0.0f;
                    state = state_hover_text;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                timer = 0.0f;
                return false;
            }
        case state_hover_text:
            if(currently_hovering_over_widget)
            {
                timer = 0.0f;
                return true;
            }
            else
            {
                timer += dt;
                if(timer > time_to_hide)
                {
                    state = state_no_hover;
                    timer = 0.0f;
                }

                return false;
            }
        default:
            DIE("invalid state");
            return false;
        }
    }
}

namespace eu::core
{

void HelpTextHover::update(bool currently_hovering_over_widget, float dt)
{
    display_hover = ::update
    (
        state,
        timer,
        time_to_hover,
        time_to_hide,
        currently_hovering_over_widget, dt
    );
}

}
