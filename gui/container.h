#ifndef GUI_CONTAINER_H
#define GUI_CONTAINER_H

#include <memory>
#include <vector>

namespace euphoria::render
{
    struct SpriteRenderer;
}

namespace euphoria::gui
{
    struct Widget;

    struct Container
    {
    public:
        Container();
        ~Container();

        bool
        HasWidgets() const;

        void
        Add(std::shared_ptr<Widget> widget);

        void
        Step(float dt);

        void
        Render(render::SpriteRenderer* renderer) const;

    protected:
        std::vector<std::shared_ptr<Widget>> widgets_;
    };
}

#endif  // GUI_CONTAINER_H
