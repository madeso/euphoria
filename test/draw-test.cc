#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"

using namespace euphoria::core;


int
main(int, char*[])
{
    auto image = Image{};
    image.SetupNoAlphaSupport(512, 512);
    
    Clear(&image, Color::White);

    const auto center = vec2i(512/2, 512/2);

    DrawCircle(&image, Color::Red, center, 512/2);

    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), "draw.png");

    return 0;
}

