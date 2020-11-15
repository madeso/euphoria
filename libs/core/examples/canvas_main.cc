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
    image.SetupNoAlphaSupport(image_size, image_size);
    Clear(&image, {Color::Yellow});

    auto ctx = Canvas{&image};

    ctx.fillStyle = Color::White;
    ctx.fillRect(0, 0, 300, 300);

    // grass
    ctx.fillStyle = Color::Green;
    ctx.fillRect(0, 200, 300, 100);

    // Wall
    ctx.fillStyle = Color::Gray;
    ctx.fillRect(75, 140, 150, 110);

    // Door
    ctx.fillStyle = Color::Black;
    ctx.fillRect(130, 190, 40, 60);

    // Roof
    ctx.fillStyle = Color::Red;
    ctx.beginPath();
    ctx.moveTo(50, 140);
    ctx.lineTo(150, 60);
    ctx.lineTo(250, 140);
    ctx.closePath();
    ctx.fill();

    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), "house.png");
    return 0;
}

