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

    auto load_image = [&]
    {
        auto ret = LoadImage(io::FileToChunk(input), input, AlphaLoad::Keep);
        if(!ret.error.empty())
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
    
    auto write_image = [&]
    {
        io::ChunkToFile(image.Write(ImageWriteFormat::PNG), output);
    };

    // todo(Gustav): change to generate/open - filter - save subparsing instead (with image targets)

    parser.Add("input", &input).Help("The image to apply filters to");
    parser.Add("-o, --output", &output)
            .Help("Where to write the resulting image");

    auto subs = parser.AddSubParsers();

    subs->Add
    (
        "nop", "Don't do anything",
        [&](argparse::SubParser* sub)
        {
            return sub->OnComplete
            (
                [&]
                {
                    if(!load_image())
                    {
                        return;
                    }
                    write_image();
                }
            );
            
        }
    );

    subs->Add
    (
        "grayscale", "Apply grayscale",
        [&](argparse::SubParser* sub)
        {
            Grayscale grayscale = Grayscale::Average;
            sub->Add("-g,--grayscale", &grayscale);
            return sub->OnComplete
            (
                [&]
                {
                    if(!load_image())
                    {
                        return;
                    }
                    MakeGrayscale(&image, grayscale);
                    write_image();
                }
            );
            
        }
    );

    subs->Add
    (
        "palswap", "Switch palette",
        [&](argparse::SubParser* sub)
        {
            palette::PaletteName palette = palette::PaletteName::OneBit;
            bool pal_dither = false;

            sub->Add("-p, --palette", &palette);
            sub->SetTrue("-d, --dither", &pal_dither);

            return sub->OnComplete
            (
                [&]
                {
                    if(!load_image())
                    {
                        return;
                    }
                    if(pal_dither)
                    {
                        MatchPaletteDither(&image, palette::GetPalette(palette));
                    }
                    else
                    {
                        MatchPalette(&image, palette::GetPalette(palette));
                    }
                    write_image();
                }
            );
            
        }
    );

    subs->Add
    (
        "edge", "Edge detection",
        [&](argparse::SubParser* sub)
        {
            float edge_r = 0.5f;
            sub->Add("-r, --range", &edge_r);
            return sub->OnComplete
            (
                [&]
                {
                    if(!load_image())
                    {
                        return;
                    }
                    EdgeDetection(&image, edge_r);
                    write_image();
                }
            );
        }
    );

    subs->Add
    (
        "color", "Detect colors",
        [&](argparse::SubParser* sub)
        {
            float edge_r = 0.5f;
            auto color_color = Color::Red;
            sub->Add("-r, --range", &edge_r);
            sub->Add("-c, --color", &color_color);
            return sub->OnComplete
            (
                [&]
                {
                    if(!load_image())
                    {
                        return;
                    }
                    ColorDetection(&image, color_color, edge_r);
                    write_image();
                }
            );
        }
    );

    subs->Add
    (
        "bright", "Change brightness",
        [&](argparse::SubParser* sub)
        {
            int bright_c = 10;
            sub->Add("-c, --change", &bright_c);
            return sub->OnComplete
            (
                [&]
                {
                    if(!load_image())
                    {
                        return;
                    }
                    ChangeBrightness(&image, bright_c);
                    write_image();
                }
            );
        }
    );

    subs->Add
    (
        "contrast", "Change contrast",
        [&](argparse::SubParser* sub)
        {
            float contrast = 10;
            sub->Add("-c, --change", &contrast);
            return sub->OnComplete
            (
                [&]
                {
                    if (!load_image())
                    {
                        return;
                    }
                    ChangeContrast(&image, contrast);
                    write_image();
                }
            );
        }
    );


    return argparse::ParseFromMain(&parser, argc, argv);
}
