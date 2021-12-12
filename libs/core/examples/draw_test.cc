#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"

using namespace euphoria::core;


int
main(int, char*[])
{
    std::array<Image, 4> images;

    constexpr int width = 200;
    constexpr int height = 200;

    for(auto& image : images)
    {
        image.setup_no_alpha_support(width, height);
        clear(&image, {NamedColor::white});
    }

    const auto center = Vec2i(width/2, height/2);

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
            const Vec2i& from,
            const Vec2i& to
        )
    {
        draw_line_antialiased(&(images[0]), color, from, to);
        draw_line_antialiased(&(images[1]), color, to, from);

        draw_line_fast(&(images[2]), {color}, from, to);
        draw_line_fast(&(images[3]), {color}, to, from);
    };

    draw(NamedColor::pure_blue, center, center + Vec2i{center.x, offset});
    draw(NamedColor::pure_yellow, center, center - Vec2i{center.x, offset});

    draw(NamedColor::pure_pink, center, center + Vec2i{offset, center.y});
    draw(NamedColor::pure_brown, center, center - Vec2i{offset, center.y});

    Image composite;
    composite.setup_no_alpha_support(width * 2, height * 2);
    for(int i=0; i<4; i+=1)
    {
        const auto p = Vec2i{i%2 * width, i/2 * height};
        paste_image(&composite, p, images[i]);
    }

    io::chunk_to_file(composite.write(ImageWriteFormat::png), "draw.png");

    return 0;
}

