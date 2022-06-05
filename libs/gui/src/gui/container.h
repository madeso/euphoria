#pragma once

#include <memory>


#include "core/noncopyable.h"


namespace euphoria::render
{
    struct SpriteRenderer;
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
        has_any_widgets() const;

        void
        add(std::shared_ptr<Widget> widget);

        void
        step(float dt);

        void
        render(render::SpriteRenderer* renderer) const;

        std::vector<std::shared_ptr<Widget>> widgets;
    };
}

