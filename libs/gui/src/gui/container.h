#pragma once

#include <memory>
#include <vector>

#include "core/noncopyable.h"


namespace euphoria::render
{
    struct SpriteRenderer;
}

namespace euphoria::gui
{
    struct widget;
}


namespace euphoria::gui
{
    struct container
    {
        container();
        ~container();

        NONCOPYABLE(container);

        [[nodiscard]] bool
        has_any_widgets() const;

        void
        add(std::shared_ptr<widget> widget);

        void
        step(float dt);

        void
        render(render::SpriteRenderer* renderer) const;

        std::vector<std::shared_ptr<widget>> widgets;
    };
}

