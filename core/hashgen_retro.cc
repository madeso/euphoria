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
        auto rand = TGenerator{code};
        const auto size = half_size * 2;

        // todo(Gustav): figure out color (randomly?)
        const auto color = Color::White;
        const auto bg = Color::Black;

        const auto dx = image->GetWidth() / (size - 1);
        const auto dy = image->GetHeight() / size;

        Clear(image, bg);

        for(int y=0; y<size; y+=1)
        for(int x=0; x<half_size; x+=1)
        {
            const auto r = rand.Next();
            const auto filled = r < 0.5f;
            if(filled)
            {
                DrawRect
                (
                    image,
                    color,
                    Recti::FromTopLeftWidthHeight
                    (
                        vec2i{x*dx, (y+1)*dy},
                        dx+1, dy
                    )
                );
                if(x != half_size-1)
                {
                    DrawRect
                    (
                        image,
                        color,
                        Recti::FromTopLeftWidthHeight
                        (
                            vec2i{image->GetWidth() - ((x+1)*dx + 2), (y+1)*dy},
                            dx+1, dy
                        )
                    );
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

