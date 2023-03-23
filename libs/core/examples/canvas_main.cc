#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/image_canvas.h"


using namespace euphoria::core;


int
main(int, char*[])
{
    constexpr auto image_size = 300;
    Image image;
    image.setup_no_alpha_support(image_size, image_size);
    clear(&image, {NamedColor::yellow});

    auto ctx = Canvas{&image};

    ctx.fill_style = NamedColor::white;
    ctx.fill_rect(0, 0, 300, 300);

    // grass
    ctx.fill_style = NamedColor::green;
    ctx.fill_rect(0, 200, 300, 100);

    // Wall
    ctx.fill_style = NamedColor::gray;
    ctx.fill_rect(75, 140, 150, 110);

    // Door
    ctx.fill_style = NamedColor::black;
    ctx.fill_rect(130, 190, 40, 60);

    // Roof
    ctx.fill_style = NamedColor::red;
    ctx.begin_path();
    ctx.move_to(50, 140);
    ctx.draw_line_to(150, 60);
    ctx.draw_line_to(250, 140);
    ctx.close_path();
    ctx.fill();

    io::write_chunk_to_file(image.write(ImageWriteFormat::png), "house.png");
    return 0;
}

