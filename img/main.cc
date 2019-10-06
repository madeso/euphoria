#include "core/argparse.h"

#include "core/image.h"
#include "core/imagefilters.h"
#include "core/palette_all.h"

#include "core/io.h"

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace euphoria::core;

int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser {"Apply filters to images"};

    std::string input;
    std::string output = "ret.png";

    Image image;

    auto load_image = [&] {
        auto ret = LoadImage(io::FileToChunk(input), input, AlphaLoad::Keep);
        if (!ret.error.empty())
        {
            std::cerr << ret.error << "\n";
            return false;
        }
        else
        {
            image = ret.image;
            return true;
        }
    };
    auto write_image = [&] {
        io::ChunkToFile(image.Write(ImageWriteFormat::PNG), output);
    };

    parser.AddSimple("input", &input).Help("The image to apply filters to");
    parser.AddSimple("-o, --output", &output)
            .Help("Where to write the resulting image");

    parser.AddSubParser("nop", "Don't do anything", [&] {
        if (!load_image())
        {
            return;
        }
        write_image();
    });

    Grayscale grayscale = Grayscale::Average;
    auto pgrayscale = parser.AddSubParser("grayscale", "Apply grayscale", [&] {
        if (!load_image())
        {
            return;
        }
        ImageFilter {&image}.MakeGrayscale(grayscale);
        write_image();
    });
    pgrayscale->AddEnum("-g,--grayscale", &grayscale);

    palette::PaletteName palette    = palette::PaletteName::OneBit;
    bool                 pal_dither = false;
    auto ppalette = parser.AddSubParser("palswap", "Switch palette", [&] {
        if (!load_image())
        {
            return;
        }
        if (pal_dither)
        {
            ImageFilter {&image}.MatchPaletteDither(
                    palette::GetPalette(palette));
        }
        else
        {
            ImageFilter {&image}.MatchPalette(palette::GetPalette(palette));
        }
        write_image();
    });
    ppalette->AddEnum("-p, --palette", &palette);
    ppalette->SetTrue("-d, --dither", &pal_dither);

    float edge_r = 0.5f;
    auto  pedge  = parser.AddSubParser("edge", "Edge detection", [&] {
        if (!load_image())
        {
            return;
        }
        ImageFilter {&image}.EdgeDetection(edge_r);
        write_image();
    });
    pedge->AddSimple("-r, --range", &edge_r);

    auto color_color = Color::Red;
    auto pcolor      = parser.AddSubParser("color", "Detect colors", [&] {
        if (!load_image())
        {
            return;
        }
        ImageFilter {&image}.ColorDetection(color_color, edge_r);
        write_image();
    });
    pcolor->AddSimple("-r, --range", &edge_r);
    pcolor->AddEnum("-c, --color", &color_color);

    int  bright_c = 10;
    auto pbright  = parser.AddSubParser("bright", "Change brightness", [&] {
        if (!load_image())
        {
            return;
        }
        ImageFilter {&image}.ChangeBrightness(bright_c);
        write_image();
    });
    pbright->AddSimple("-c, --change", &bright_c);

    float contrast = 10;
    auto  pcontr   = parser.AddSubParser("contrast", "Change contrast", [&] {
        if (!load_image())
        {
            return;
        }
        ImageFilter {&image}.ChangeContrast(contrast);
        write_image();
    });
    pcontr->AddSimple("-c, --change", &contrast);


    const auto status = parser.Parse(argc, argv);
    if (status != argparse::ParseResult::Ok)
    {
        return -1;
    }


    return 0;
}
