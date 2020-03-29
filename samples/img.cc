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
    Image image;

    auto parser = argparse::Parser {"Apply filters to images"};
    parser.parser_style = argparse::SubParserStyle::Fallback;

    auto io_subs = parser.AddSubParsers("transfer image to/from disk");

    io_subs->Add
    (
        "open", "Load image from disk",
        [&](argparse::SubParser* sub)
        {
            std::string input;
            sub->Add("input", &input).Help("The image to apply filters to");
            return sub->OnComplete
            (
                [&]
                {
                    auto file = io::FileToChunk(input);
                    if(file == nullptr)
                    {
                        std::cerr << "Failed to open image file: '" << input << "'\n";
                        return argparse::ParseResult::Error;
                    }
                    auto ret = LoadImage(file, input, AlphaLoad::Keep);
                    if(!ret.error.empty())
                    {
                        std::cerr << ret.error << "\n";
                        return argparse::ParseResult::Error;
                    }
                    else
                    {
                        image = ret.image;
                        return argparse::ParseResult::Ok;
                    }
                }
            );
        }
    );

    io_subs->Add
    (
        "save", "Write image to disk",
        [&](argparse::SubParser* sub)
        {
            std::string output = "ret.png";
            sub->Add("-o, --output", &output).Help
            (
                "Where to write the resulting image"
            );
            return sub->OnComplete
            (
                [&]
                {
                    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), output);
                    return argparse::ParseResult::Ok;
                }
            );
            
        }
    );

    auto filters_subs = parser.AddSubParsers("apply filter to current image");

    filters_subs->Add
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
                    MakeGrayscale(&image, grayscale);
                    return argparse::ParseResult::Ok;
                }
            );
            
        }
    );

    filters_subs->Add
    (
        "palswap", "Switch palette",
        [&](argparse::SubParser* sub)
        {
            auto palette_name = palette::PaletteName::OneBit;
            auto dither = false;

            sub->Add("-p, --palette", &palette_name);
            sub->SetTrue("-d, --dither", &dither);

            return sub->OnComplete
            (
                [&]
                {
                    const auto& palette = palette::GetPalette(palette_name);

                    if(dither)
                    {
                        MatchPaletteDither(&image, palette);
                    }
                    else
                    {
                        MatchPalette(&image, palette);
                    }

                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    filters_subs->Add
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
                    EdgeDetection(&image, edge_r);
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    filters_subs->Add
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
                    ColorDetection(&image, color_color, edge_r);
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    filters_subs->Add
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
                    ChangeBrightness(&image, bright_c);
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    filters_subs->Add
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
                    ChangeContrast(&image, contrast);
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    return argparse::ParseFromMain(&parser, argc, argv);
}
