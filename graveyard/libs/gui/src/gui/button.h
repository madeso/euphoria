#pragma once

#include "base/rect.h"
#include "base/rgb.h"
#include "base/vec2.h"
#include "core/tweenable.h"

#include "gui/widget.h"
#include "gui/textdata.h"

#include <memory>


namespace eu::render
{
    struct ScalableSprite;
}

namespace eu::gui
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

        core::Tweenable<float> scale;
        core::Tweenable<Rgb> image_color;
        core::Tweenable<Rgb> text_color;
        core::Tweenable<vec2f> position_displacement;

        Button(gui::State* state);
        ~Button() override;

        Button(const Button&) = delete;
        Button(Button&&) = delete;
        void operator=(const Button&) = delete;
        void operator=(Button&&) = delete;

        void set_skin(Skin* new_skin);

        void update(float dt) override;
        [[nodiscard]] size2f calc_minimum_size() const override;
        void render(render::SpriteRenderer* renderer) const override;
        void visit(Visitor* visitor) override;
        void on_size_changed() override;

        virtual void on_clicked() = 0;
    };
}
