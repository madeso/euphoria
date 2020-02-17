#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/argparse.h"
#include "core/palette_all.h"
#include "core/palette.h"
#include "core/str.h"

using namespace euphoria::core;

int
main(int argc, char* argv[])
{
    auto image_size = 32;

    auto parser = argparse::Parser {"euphoria raytracer"};
    parser.AddSimple("-size", &image_size).Help("Minimum image size");

    if(parser.Parse(argc, argv) != argparse::ParseResult::Ok)
    {
        return -1;
    }

    for (auto palette_name : palette::PaletteNames)
    {
        const auto pal = palette::GetPalette(palette_name);

        Image image;
        image.SetupNoAlphaSupport(image_size * pal.colors.size(), image_size);

        for (int i = 0; i < pal.colors.size(); i += 1)
        {
            DrawSquare(&image, pal.colors[i], image_size * i, image_size-1, image_size);
        }

        const std::string file = Str{} << "palette_" << pal.name << ".png";

        io::ChunkToFile(image.Write(ImageWriteFormat::PNG), file);
    }

    return 0;
}

