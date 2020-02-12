#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/identicon.h"

using namespace euphoria::core;


int
main(int, char*[])
{
    constexpr auto image_size = 512;
    Image image;
    image.SetupNoAlphaSupport(image_size, image_size);

    RenderIdenticon(&image, -2044886870);
    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), "identicon.png");

    return 0;
}

