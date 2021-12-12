#pragma once

#include "core/rect.h"
#include "core/rgb.h"
#include "core/vec2.h"
#include "core/interpolate.h"
#include "core/noncopyable.h"

#include "gui/widget.h"
#include "gui/textdata.h"

#include <memory>


namespace euphoria::render
{
    struct scalable_sprite;
}

namespace euphoria::gui
{
    struct skin;
    struct button_state;
    struct ui_state;

    struct button : widget
    {
         button(gui::ui_state* state);
        ~button() override;

        NONCOPYABLE(button);

        virtual void
        on_clicked() = 0;

        void
        step(float dt) override;

        [[nodiscard]] core::size2f
        calculate_minimum_size() const override;

        void
        render(render::sprite_renderer* renderer) const override;

        void
        visit(visitor* visitor) override;

        void
        on_size_changed() override;

        void
        set_skin(skin* new_skin);

        button_state* last_state;
        std::shared_ptr<render::scalable_sprite> sprite;
        text_data text;
        skin* skin_;

        core::Interpolate<float, core::FloatTransform> scale;
        core::Interpolate<core::Rgb, core::RgbTransform> image_color;
        core::Interpolate<core::Rgb, core::RgbTransform> text_color;
        core::Interpolate<core::Vec2f, core::Vec2fTransform> position_displacement;
    };
}
