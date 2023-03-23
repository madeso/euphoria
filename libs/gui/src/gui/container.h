#pragma once

#include <memory>


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
        std::vector<std::shared_ptr<Widget>> widgets;

        Container();
        ~Container();

        Container(const Container&) = delete;
        Container(Container&&) = delete;
        void operator=(const Container&) = delete;
        void operator=(Container&&) = delete;

        void add(std::shared_ptr<Widget> widget);
        void update(float dt);

        void render(render::SpriteRenderer* renderer) const;

        [[nodiscard]] bool has_any_widgets() const;
    };
}

