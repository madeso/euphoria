#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"

using namespace euphoria::core;


int
main(int, char*[])
{
    std::array<Image, 4> images;

    constexpr auto width = 200;
    constexpr auto height = 200;

    for(auto& image : images)
    {
        image.SetupNoAlphaSupport(width, height);
        Clear(&image, {Color::White});
    }

    const auto center = vec2i(width/2, height/2);

    for(auto& image : images)
    {
        DrawCircle(&image, Color::Red, center, width/2);
    }

    constexpr int offset = 20;

    // lines:
    // fast | fast reversed
    // anti | anti reversed

    auto draw = [&]
        (
            const Color color,
            const vec2i& from,
            const vec2i& to
        )
    {
        DrawLineAntialiased(&(images[0]), color, from, to);
        DrawLineAntialiased(&(images[1]), color, to, from);

        DrawLineFast(&(images[2]), {color}, from, to);
        DrawLineFast(&(images[3]), {color}, to, from);
    };

    draw(Color::PureBlue, center, center + vec2i{center.x, offset});
    draw(Color::PureYellow, center, center - vec2i{center.x, offset});

    draw(Color::PurePink, center, center + vec2i{offset, center.y});
    draw(Color::PureBrown, center, center - vec2i{offset, center.y});

    Image composite;
    composite.SetupNoAlphaSupport(width * 2, height * 2);
    for(int i=0; i<4; i+=1)
    {
        const auto p = vec2i{i%2 * width, i/2 * height};
        PasteImage(&composite, p, images[i]);
    }

    io::ChunkToFile(composite.Write(ImageWriteFormat::PNG), "draw.png");

    return 0;
}

