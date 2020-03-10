#include "core/hashgen_retro.h"

#include <vector>

#include "core/image.h"
#include "core/rgb.h"
#include "core/assert.h"
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
    RenderRetroImpl
    (
        Image* image,
        I code,
        int half_size = 3
    )
    {
        const auto size = half_size * 2;

        // todo(Gustav): figure out color (randomly?)
        const auto foreground_color = Color::White;
        const auto background_color = Color::Black;

        const auto dx = image->GetWidth() / (size - 1);
        const auto dy = image->GetHeight() / size;

        auto draw_rect = [&](const vec2i& top_left)
        {
            DrawRect
            (
                image,
                foreground_color,
                Recti::FromTopLeftWidthHeight
                (
                    top_left,
                    dx+1, dy
                )
            );
        };

        Clear(image, background_color);

        auto generator = TGenerator{code};

        for(int y=0; y<size; y+=1)
        for(int x=0; x<half_size; x+=1)
        {
            const auto filled = generator.Next() < 0.5f;
            if(filled)
            {
                draw_rect(vec2i{x*dx, (y+1)*dy});
                if(x != half_size-1)
                {
                    draw_rect(vec2i{image->GetWidth() - ((x+1)*dx + 2), (y+1)*dy});
                }
            }
        }
    }
}


namespace euphoria::core
{
    void
    RenderRetro(Image* image, int code)
    {
        RenderRetroImpl<xorshift32>(image, Cbit_signed_to_unsigned(code));
    }
}

