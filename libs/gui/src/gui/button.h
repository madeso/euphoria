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
    struct ScalableSprite;
}

namespace euphoria::gui
{
    struct Skin;
    struct ButtonState;
    struct State;

    struct Button : Widget
    {
        ButtonState* last_state;
        std::shared_ptr<render::ScalableSprite> sprite;
        TextData text;
        Skin* skin;

        core::Interpolate<float, core::FloatTransform> scale;
        core::Interpolate<core::rgb, core::RgbTransform> image_color;
        core::Interpolate<core::rgb, core::RgbTransform> text_color;
        core::Interpolate<core::vec2f, core::Vec2fTransform> position_displacement;

        Button(gui::State* state);
        ~Button() override;

        Button(const Button&) = delete;
        Button(Button&&) = delete;
        void operator=(const Button&) = delete;
        void operator=(Button&&) = delete;

        void set_skin(Skin* new_skin);

        void step(float dt) override;
        [[nodiscard]] core::size2f calculate_minimum_size() const override;
        void render(render::SpriteRenderer* renderer) const override;
        void visit(Visitor* visitor) override;
        void on_size_changed() override;

        virtual void on_clicked() = 0;
    };
}
