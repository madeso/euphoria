#include "render/viewporthandler.h"

#include "core/viewportdef.h"
#include "base/cint.h"
#include "core/viewport.h"

#include "render/viewport.h"
#include "render/init.h"
#include "render/shader.h"


namespace
{
    namespace core = eu::core;

    core::ViewportDefinition
    create_definition(const eu::render::ViewportHandler& handler)
    {
        switch(handler.type)
        {
        case eu::render::ViewportType::fit_with_black_bars:
            return core::ViewportDefinition::from_fit_with_black_bars
            (
                handler.virtual_width,
                handler.virtual_height,
                handler.window_width,
                handler.window_height
            );
        case eu::render::ViewportType::screen_pixel:
            return core::ViewportDefinition::from_screen_pixel
            (
                handler.window_width,
                handler.window_height
            );
        case eu::render::ViewportType::extend:
            return core::ViewportDefinition::from_extend
            (
                handler.virtual_width,
                handler.virtual_height,
                handler.window_width,
                handler.window_height
            );
        default:
            DIE("Unhandled viewport case");
            return core::ViewportDefinition::from_screen_pixel
            (
                handler.window_width,
                handler.window_height
            );
        }
    }


    void apply_viewport
    (
        eu::render::ViewportHandler* handler,
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
            *handler->virtual_screen = eu::Rectf::from_width_height
            (
                vp.virtual_width,
                vp.virtual_height
            );
        }

        auto viewport = eu::core::Viewport{vp.screen_rect};
        eu::render::activate(viewport);
    }
}


namespace eu::render
{
    ViewportHandler::ViewportHandler(render::Init* i, Rectf* s)
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
            core::ViewportDefinition::from_screen_pixel(window_width, window_height),
            false
        );

        init->clear_screen(NamedColor::black);

        apply_viewport
        (
                this,
                create_definition(*this),
                false
        );
    }



    core::Viewport
    ViewportHandler::get_full_viewport() const
    {
        const auto viewport = core::Viewport
        {
            Recti::from_width_height(window_width, window_height)
                .set_bottom_left_to_copy
                (
                    0,
                    0
                )
        };
        return viewport;
    }
}

