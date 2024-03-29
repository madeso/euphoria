#pragma once


namespace eu::gui
{
    struct Button;
    struct PanelWidget;
}


namespace eu::gui
{
    struct Visitor
    {
        Visitor() = default;
        virtual ~Visitor() = default;
        Visitor(const Visitor& other) = delete;
        void operator=(const Visitor&) = delete;
        Visitor(Visitor&& other) = delete;
        void operator=(Visitor&&) = delete;

        virtual
        void
        visit(Button* button) = 0;

        virtual
        void
        visit(PanelWidget* button) = 0;
    };
}

