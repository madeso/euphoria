#include "core/image.h"
#include "core/image_draw.h"
#include "io/io.h"

using namespace eu::core;
using namespace eu::io;


int
main(int, char*[])
{
    Image images[4];

    constexpr int width = 200;
    constexpr int height = 200;

    for(auto& image : images)
    {
        image.setup_no_alpha_support(width, height);
        clear(&image, {NamedColor::white});
    }

    const auto center = vec2i(width/2, height/2);

    for(auto& image : images)
    {
        draw_circle(&image, NamedColor::red, center, static_cast<float>(width)/2.0f);
    }

    constexpr int offset = 20;

    // lines:
    // fast | fast reversed
    // anti | anti reversed

    auto draw = [&]
        (
            const NamedColor color,
            const vec2i& from,
            const vec2i& to
        )
    {
        draw_line_antialiased(&(images[0]), color, from, to);
        draw_line_antialiased(&(images[1]), color, to, from);

        draw_line_fast(&(images[2]), {color}, from, to);
        draw_line_fast(&(images[3]), {color}, to, from);
    };

    draw(NamedColor::pure_blue, center, center + vec2i{center.x, offset});
    draw(NamedColor::pure_yellow, center, center - vec2i{center.x, offset});

    draw(NamedColor::pure_pink, center, center + vec2i{offset, center.y});
    draw(NamedColor::pure_brown, center, center - vec2i{offset, center.y});

    Image composite;
    composite.setup_no_alpha_support(width * 2, height * 2);
    for(int image_index=0; image_index<4; image_index+=1)
    {
        const auto p = vec2i{image_index%2 * width, image_index/2 * height};
        paste_image(&composite, p, images[image_index]);
    }

    write_chunk_to_file(composite.write(ImageWriteFormat::png), "draw.png");

    return 0;
}

