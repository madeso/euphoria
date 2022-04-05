#pragma once


namespace euphoria::core
{
    /** Determines when to display help text in a hover ui
     */
    struct HelpTextHover
    {
        float time_to_hover = 0.8;
        float time_to_hide = 0.5f;

        int state = 0;
        float timer = 0.0f;

        void step(bool currently_hovering_over_widget, float dt);
        bool display_hover = false;
    };
}
