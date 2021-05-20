#pragma once

#include <memory>
#include <vector>

#include "core/noncopyable.h"


namespace euphoria::render
{
    struct sprite_renderer;
}

namespace euphoria::gui
{
    struct Widget;
}


namespace euphoria::gui
{
    struct Container
    {
        Container();
        ~Container();

        NONCOPYABLE(Container);

        [[nodiscard]] bool
        HasWidgets() const;

        void
        Add(std::shared_ptr<Widget> widget);

        void
        Step(float dt);

        void
        Render(render::sprite_renderer* renderer) const;

        std::vector<std::shared_ptr<Widget>> widgets_;
    };
}

