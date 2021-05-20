#include "render/viewporthandler.h"

#include "core/viewportdef.h"
#include "core/cint.h"

#include "render/viewport.h"
#include "render/init.h"
#include "render/shader.h"


namespace
{
    namespace core = euphoria::core;

    core::viewport_definition
    CreateDefinition(const euphoria::render::viewport_handler& handler)
    {
        switch(handler.type)
        {
        case euphoria::render::viewport_type::fit_with_black_bars:
            return core::viewport_definition::fit_with_black_bars
            (
                handler.virtual_width,
                handler.virtual_height,
                handler.window_width,
                handler.window_height
            );
        case euphoria::render::viewport_type::screen_pixel:
            return core::viewport_definition::screen_pixel
            (
                handler.window_width,
                handler.window_height
            );
        case euphoria::render::viewport_type::extend:
            return core::viewport_definition::extend
            (
                handler.virtual_width,
                handler.virtual_height,
                handler.window_width,
                handler.window_height
            );
        default:
            DIE("Unhandled viewport case");
            return core::viewport_definition::screen_pixel
            (
                handler.window_width,
                handler.window_height
            );
        }
    }


    void Apply
    (
        euphoria::render::viewport_handler* handler,
        const core::viewport_definition& vp,
        bool shaders_too
    )
    {
        if(shaders_too)
        {
            const auto projection = handler->init->get_ortho_projection
            (
                vp.virtual_width,
                vp.virtual_height
            );
            for(auto* shader: handler->shaders)
            {
                shader->set_uniform
                (
                    shader->get_uniform("projection"),
                    projection
                );
            }
        }

        if(handler->virtual_screen != nullptr)
        {
            *handler->virtual_screen = core::rectf::from_width_height
            (
                vp.virtual_width,
                vp.virtual_height
            );
        }

        auto viewport = euphoria::render::viewport{vp.screen_rect};
        viewport.activate();
    }
}


namespace euphoria::render
{
    viewport_handler::viewport_handler(render::init* i, core::rectf* s)
        : init(i)
        , virtual_screen(s)
    {
    }


    void
    viewport_handler::add(shader* shader)
    {
        shaders.emplace_back(shader);
    }


    void
    viewport_handler::set_size(int width, int height)
    {
        window_width = width;
        window_height = height;

        const auto vp = CreateDefinition(*this);

        Apply(this, vp, true);
    }


    void
    viewport_handler::clear_black()
    {
        if(type != viewport_type::fit_with_black_bars) { return; }

        Apply
        (
            this,
            core::viewport_definition::screen_pixel(window_width, window_height),
            false
        );

        init->clear_screen(core::color::black);

        Apply
        (
            this,
            CreateDefinition(*this),
            false
        );
    }



    viewport
    viewport_handler::get_full_viewport() const
    {
        const auto viewport = render::viewport
        {
            core::recti::from_width_height(window_width, window_height)
                .set_bottom_left_to_copy
                (
                    0,
                    0
                )
        };
        return viewport;
    }
}

