#include "core/hashgen.h"



#include "core/image.h"
#include "core/rgb.h"
#include "assert/assert.h"
#include "core/rng.h"
#include "core/numeric.h"
#include "core/image_draw.h"
#include "core/cint.h"


namespace
{
    using namespace euphoria::core;

    template
    <
        typename TGenerator,
        typename I
    >
    void
    render_retro_impl
    (
        Image* image,
        I code,
        int half_size = 3
    )
    {
        const auto size = half_size * 2;

        // todo(Gustav): figure out color (randomly?)
        const auto foreground_color = NamedColor::white;
        const auto background_color = NamedColor::black;

        const auto dx = image->width / (size - 1);
        const auto dy = image->height / size;

        auto rect = [&](const vec2i& top_left)
        {
            draw_rect
            (
                image,
                {foreground_color},
                Recti::from_top_left_width_height
                (
                    top_left,
                    dx+1, dy
                )
            );
        };

        clear(image, {background_color});

        auto generator = TGenerator{code};

        for(int y=0; y<size; y+=1)
        {
            for(int x=0; x<half_size; x+=1)
            {
                const auto filled = generator.get_next_float01() < 0.5f;
                if(filled)
                {
                    rect(vec2i{x*dx, (y+1)*dy});
                    if(x != half_size-1)
                    {
                        rect(vec2i{image->width - ((x+1)*dx + 2), (y+1)*dy});
                    }
                }
            }
        }
    }
}


namespace euphoria::core
{
    void
    render_retro(Image* image, U32 code)
    {
        render_retro_impl<RandomXorShift32>(image, code);
    }
}

