#include "gui/button.h"
#include "gui/uistate.h"

#include "render/scalablesprite.h"
#include "render/spriterender.h"
#include "render/font.h"

#include "gui/visitor.h"
#include "gui/skin.h"


namespace eu::gui
{
    Button::Button(gui::State* state)
        : Widget(state)
        , last_state(nullptr)
        , skin(nullptr)
        , scale(1.0f)
        , image_color(core::Rgb(1.0f))
        , text_color(core::Rgb(1.0f))
        , position_displacement(zero2f)
    {
    }


    Button::~Button() = default;


    void
    Button::update(float dt)
    {
        if(get_background_rect().contains_exclusive(get_state().mouse))
        {
            get_state_ptr()->set_hot(this);
            if(get_state().is_mouse_down())
            {
                get_state_ptr()->set_active(this);
            }
        }

        const bool mouse_down = get_state().is_mouse_down();
        const bool is_clicked = mouse_down == false && is_hot() && is_active();
        if(is_clicked)
        {
            on_clicked();
        }

        if(skin != nullptr)
        {
            auto* cold_state = &skin->button_idle;
            auto* hot_state = is_active()
                ? &skin->button_active_hot
                : &skin->button_hot
                ;
            auto* state = is_hot() ? hot_state : cold_state;

            if(last_state != state)
            {
                last_state = state;
                scale.set
                (
                    state->interpolation_size.type,
                    state->scale,
                    state->interpolation_size.time
                );
                image_color.set
                (
                    state->interpolation_color.type,
                    state->image_color,
                    state->interpolation_color.time
                );
                text_color.set
                (
                    state->interpolation_color.type,
                    state->text_color,
                    state->interpolation_color.time
                );
                position_displacement.set
                (
                    state->interpolation_position.type,
                    vec2f(state->dx, state->dy),
                    state->interpolation_position.time
                );
            }

            scale.update(dt);
            image_color.update(dt);
            text_color.update(dt);
            position_displacement.update(dt);
            text.set_size(skin->text_size * scale.value);
        }
    }


    size2f
    Button::calc_minimum_size() const
    {
        auto size = size2f::create_from_width_height(0, 0);
        if(sprite != nullptr)
        {
            const auto ms = sprite->get_minimum_size();
            size.width = (size.width + ms.width);
            size.height = (size.height + ms.height);
        }

        if(text.has_text())
        {
            const auto extents = text.get_text().get_extents();
            const auto ms = size2f::create_from_width_height
            (
                extents.get_width(),
                extents.get_height()
            );
            size.width = size.width + ms.width;
            size.height = size.height + ms.height;
        }

        return size;
    }


    void
    Button::render(render::SpriteRenderer* renderer) const
    {
        if(skin != nullptr)
        {
            if(sprite != nullptr)
            {
                const auto background_rect = get_background_rect();

                ASSERTX(scale.value > 0, scale.value);
                const auto scaled = background_rect.get_scaled_around_center_copy
                (
                    scale.value
                );
                ASSERTX(scaled.get_width() > 0, scaled.get_width());
                ASSERTX(scaled.get_height() > 0, scaled.get_height());
                renderer->draw_ninepatch
                (
                    *sprite,
                    scaled.translate_copy(position_displacement.value),
                    image_color.value
                );
            }

            if(text.has_text())
            {
                const auto ex = text.get_text().get_extents();
                // todo(Gustav): render text at client Rect center

                const auto base = ex.center_inside_other(get_client_rect());
                const auto base_pos = base.get_bottom_left();
                const auto p = base_pos + position_displacement.value;
                text.get_text().draw(renderer, p, text_color.value);
            }
        }
    }


    void
    Button::visit(Visitor* visitor)
    {
        visitor->visit(this);
    }


    void
    Button::on_size_changed()
    {
    }


    void
    Button::set_skin(Skin* new_skin)
    {
        skin = new_skin;
        if(skin != nullptr)
        {
            text.set_size(skin->text_size);
        }
    }
}

