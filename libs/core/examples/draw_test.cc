#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"

using namespace euphoria::core;


int
main(int, char*[])
{
    std::array<image, 4> images;

    constexpr int width = 200;
    constexpr int height = 200;

    for(auto& image : images)
    {
        image.setup_no_alpha_support(width, height);
        clear(&image, {color::white});
    }

    const auto center = vec2i(width/2, height/2);

    for(auto& image : images)
    {
        draw_circle(&image, color::red, center, static_cast<float>(width)/2.0f);
    }

    constexpr int offset = 20;

    // lines:
    // fast | fast reversed
    // anti | anti reversed

    auto draw = [&]
        (
            const color color,
            const vec2i& from,
            const vec2i& to
        )
    {
        draw_line_antialiased(&(images[0]), color, from, to);
        draw_line_antialiased(&(images[1]), color, to, from);

        draw_line_fast(&(images[2]), {color}, from, to);
        draw_line_fast(&(images[3]), {color}, to, from);
    };

    draw(color::pure_blue, center, center + vec2i{center.x, offset});
    draw(color::pure_yellow, center, center - vec2i{center.x, offset});

    draw(color::pure_pink, center, center + vec2i{offset, center.y});
    draw(color::pure_brown, center, center - vec2i{offset, center.y});

    image composite;
    composite.setup_no_alpha_support(width * 2, height * 2);
    for(int i=0; i<4; i+=1)
    {
        const auto p = vec2i{i%2 * width, i/2 * height};
        paste_image(&composite, p, images[i]);
    }

    io::chunk_to_file(composite.write(image_write_format::png), "draw.png");

    return 0;
}

