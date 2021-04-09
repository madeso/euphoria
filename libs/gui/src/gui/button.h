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
}

namespace euphoria::gui
{
    struct Button : public Widget
    {
        Button(UiState* state);
        ~Button() override;

        NONCOPYABLE(Button);

        virtual void
        OnClicked() = 0;

        void
        Step(float dt) override;

        [[nodiscard]] core::Sizef
        CalculateMinimumSize() const override;

        void
        Render(render::SpriteRenderer* renderer) const override;

        void
        Visit(Visitor* visitor) override;

        TextData&
        Text();

        void
        SetSprite(std::shared_ptr<render::ScalableSprite> sprite);

        void
        OnSize() override;

        void
        SetSkin(Skin* skin);

        ButtonState* last_state_;
        std::shared_ptr<render::ScalableSprite> sprite_;
        TextData text_;
        Skin* skin_;

        core::Interpolate<float, core::FloatTransform> scale_;
        core::Interpolate<core::Rgb, core::RgbTransform> image_color_;
        core::Interpolate<core::Rgb, core::RgbTransform> text_color_;
        core::Interpolate<core::vec2f, core::Vec2fTransform> position_displacement_;
    };
}
