#include "render/viewporthandler.h"

#include "core/viewportdef.h"
#include "core/cint.h"

#include "render/viewport.h"
#include "render/init.h"
#include "render/shader.h"


namespace
{
    namespace core = euphoria::core;

    core::ViewportDefinition
    create_definition(const euphoria::render::ViewportHandler& handler)
    {
        switch(handler.type)
        {
        case euphoria::render::ViewportType::fit_with_black_bars:
            return core::ViewportDefinition::fit_with_black_bars
            (
                handler.virtual_width,
                handler.virtual_height,
                handler.window_width,
                handler.window_height
            );
        case euphoria::render::ViewportType::screen_pixel:
            return core::ViewportDefinition::screen_pixel
            (
                handler.window_width,
                handler.window_height
            );
        case euphoria::render::ViewportType::extend:
            return core::ViewportDefinition::extend
            (
                handler.virtual_width,
                handler.virtual_height,
                handler.window_width,
                handler.window_height
            );
        default:
            DIE("Unhandled viewport case");
            return core::ViewportDefinition::screen_pixel
            (
                handler.window_width,
                handler.window_height
            );
        }
    }


    void apply_viewport
    (
        euphoria::render::ViewportHandler* handler,
        const core::ViewportDefinition& vp,
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
            *handler->virtual_screen = core::Rectf::from_width_height
            (
                vp.virtual_width,
                vp.virtual_height
            );
        }

        auto viewport = euphoria::render::Viewport{vp.screen_rect};
        viewport.activate();
    }
}


namespace euphoria::render
{
    ViewportHandler::ViewportHandler(render::Init* i, core::Rectf* s)
        : init(i)
        , virtual_screen(s)
    {
    }


    void
    ViewportHandler::add(ShaderProgram* shader)
    {
        shaders.emplace_back(shader);
    }


    void
    ViewportHandler::set_size(int width, int height)
    {
        window_width = width;
        window_height = height;

        const auto vp = create_definition(*this);

        apply_viewport(this, vp, true);
    }


    void
    ViewportHandler::clear_black()
    {
        if(type != ViewportType::fit_with_black_bars) { return; }

        apply_viewport
        (
            this,
            core::ViewportDefinition::screen_pixel(window_width, window_height),
            false
        );

        init->clear_screen(core::NamedColor::black);

        apply_viewport
        (
                this,
                create_definition(*this),
                false
        );
    }



    Viewport
    ViewportHandler::get_full_viewport() const
    {
        const auto viewport = render::Viewport
        {
            core::Recti::from_width_height(window_width, window_height)
                .set_bottom_left_to_copy
                (
                    0,
                    0
                )
        };
        return viewport;
    }
}

