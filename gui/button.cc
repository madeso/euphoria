#include "gui/button.h"
#include "uistate.h"

#include "render/scalablesprite.h"
#include "render/spriterender.h"
#include "render/fonts.h"

#include "gui/visitor.h"

#include "skin.h"

namespace euphoria::gui
{
    Button::Button(UiState* state)
        : Widget(state)
        , last_state_(nullptr)
        , scale_(1.0f)
        , image_color_(core::Rgb(1.0f))
        , text_color_(core::Rgb(1.0f))
        , position_displacement_(core::vec2f::Zero())
    {
    }


    Button::~Button()
    {
    }


    void
    Button::Step(float dt)
    {
        if(GetBackgroundRect().ContainsExclusive(GetState().mouse))
        {
            GetStatePtr()->SetHot(this);
            if(GetState().IsMouseDown())
            {
                GetStatePtr()->SetActive(this);
            }
        }

        const bool mouse_down = GetState().IsMouseDown();
        const bool is_clicked = mouse_down == false && IsHot() && IsActive();
        if(is_clicked)
        {
            OnClicked();
        }

        if(skin_)
        {
            auto* cold_state = &skin_->button_idle;
            auto* hot_state = IsActive()
                ? &skin_->button_active_hot
                : &skin_->button_hot
                ;
            auto* state = IsHot() ? hot_state : cold_state;

            if(last_state_ != state)
            {
                last_state_ = state;
                scale_.Clear().Add
                (
                    state->interpolation_size.type,
                    state->scale,
                    state->interpolation_size.time
                );
                image_color_.Clear().Add
                (
                    state->interpolation_color.type,
                    state->image_color,
                    state->interpolation_color.time
                );
                text_color_.Clear().Add
                (
                    state->interpolation_color.type,
                    state->text_color,
                    state->interpolation_color.time
                );
                position_displacement_.Clear().Add
                (
                    state->interpolation_position.type,
                    core::vec2f(state->dx, state->dy),
                    state->interpolation_position.time
                );
            }

            scale_.Update(dt);
            image_color_.Update(dt);
            text_color_.Update(dt);
            position_displacement_.Update(dt);
            text_.SetSize(skin_->text_size * scale_.GetValue());
        }
    }


    core::Sizef
    Button::CalculateMinimumSize() const
    {
        auto size = core::Sizef::FromWidthHeight(0, 0);
        if(sprite_ != nullptr)
        {
            const auto ms = sprite_->GetMinimumSize();
            size.width = (size.width + ms.width);
            size.height = (size.height + ms.height);
        }

        if(text_.HasText())
        {
            const auto extents = text_.GetText().GetExtents();
            const auto ms = core::Sizef::FromWidthHeight
            (
                extents.GetWidth(),
                extents.GetHeight()
            );
            size.width = size.width + ms.width;
            size.height = size.height + ms.height;
        }

        return size;
    }


    void
    Button::Render(render::SpriteRenderer* renderer) const
    {
        if(skin_)
        {
            if(sprite_ != nullptr)
            {
                const auto rect = GetBackgroundRect();

                ASSERTX(scale_.GetValue() > 0, scale_.GetValue());
                const auto scaled = rect.GetScaledAroundCenterCopy
                (
                    scale_.GetValue()
                );
                ASSERTX(scaled.GetWidth() > 0, scaled.GetWidth());
                ASSERTX(scaled.GetHeight() > 0, scaled.GetHeight());
                renderer->DrawNinepatch
                (
                    *sprite_.get(),
                    scaled.OffsetCopy(position_displacement_.GetValue()),
                    image_color_.GetValue()
                );
            }

            if(text_.HasText())
            {
                const auto ex = text_.GetText().GetExtents();
                // todo: render text at client rect center

                const auto base = ex.CenterInsideOther(GetClientRect());
                const auto base_pos = base.GetBottomLeft();
                const auto p = base_pos + position_displacement_.GetValue();
                text_.GetText().Draw(renderer, p, text_color_.GetValue());
            }
        }
    }


    void
    Button::Visit(Visitor* visitor)
    {
        visitor->Visit(this);
    }


    TextData&
    Button::Text()
    {
        return text_;
    }


    void
    Button::SetSprite(std::shared_ptr<render::ScalableSprite> sprite)
    {
        sprite_ = sprite;
    }


    void
    Button::OnSize()
    {
    }


    void
    Button::SetSkin(Skin* skin)
    {
        skin_ = skin;
        if(skin_)
        {
            text_.SetSize(skin_->text_size);
        }
    }
}

