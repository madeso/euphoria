#ifndef EUPHORIA_GUI_VISITOR_H
#define EUPHORIA_GUI_VISITOR_H


namespace euphoria::gui
{
    struct Button;
    struct PanelWidget;


    struct Visitor
    {
        virtual 
        ~Visitor() = default;

        virtual
        void
        Visit(Button* button) = 0;

        virtual
        void
        Visit(PanelWidget* button) = 0;
    };
}


#endif  // EUPHORIA_GUI_VISITOR_H

