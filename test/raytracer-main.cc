#include <array>

#include "core/image.h"
#include "core/io.h"
#include "core/raytracer.h"
#include "core/argparse.h"

using namespace euphoria::core;

int
main(int argc, char* argv[])
{
    auto image_size = 512;

    auto parser = argparse::Parser {"euphoria raytracer"};
    parser.AddSimple("-size", &image_size).Help("image size");

    if(parser.Parse(argc, argv) != argparse::ParseResult::Ok)
    {
        return -1;
    }

    Image image;
    image.SetupNoAlphaSupport(image_size, image_size);

    Raytrace(&image);

    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), "raytracer.png");

    return 0;
}

