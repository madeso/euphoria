#include "core/argparse.h"

#include <iostream>

#include <iomanip>

#include "core/image.h"
#include "core/imagefilters.h"
#include "core/palette_all.h"
#include "core/io.h"
#include "core/palette.h"
#include "core/palette_extract.h"


using namespace euphoria::core;


int
main(int argc, char* argv[])
{
    Image image;

    auto parser = argparse::Parser {"Apply filters to images"};
    parser.parser_style = argparse::SubParserStyle::fallback;

    auto io_subs = parser.add_sub_parsers("transfer image to/from disk");

    io_subs->add
    (
        "open", "Load image from disk",
        [&](argparse::SubParser* sub)
        {
            std::string input;
            sub->add("input", &input).set_help("The image to apply filters to");
            return sub->on_complete
            (
                [&]
                {
                    auto file = io::file_to_chunk(input);
                    if(file == nullptr)
                    {
                        std::cerr
                            << "Failed to open image file: '"
                            << input << "'\n"
                            ;
                        return argparse::error;
                    }
                    auto ret = load_image(file, input, AlphaLoad::keep);
                    if(!ret.error.empty())
                    {
                        std::cerr << ret.error << "\n";
                        return argparse::error;
                    }
                    else
                    {
                        image = ret.image;
                        return argparse::ok;
                    }
                }
            );
        }
    );

    io_subs->add
    (
        "save", "Write image to disk",
        [&](argparse::SubParser* sub)
        {
            std::string output = "ret.png";
            sub->add("-o, --output", &output).set_help
            (
                "Where to write the resulting image"
            );
            return sub->on_complete
            (
                [&]
                {
                    io::chunk_to_file
                    (
                        image.write(ImageWriteFormat::png),
                        output
                    );
                    return argparse::ok;
                }
            );

        }
    );

    auto filters_subs = parser.add_sub_parsers("apply filter to current image");

    filters_subs->add
    (
        "grayscale", "Apply grayscale",
        [&](argparse::SubParser* sub)
        {
            Grayscale grayscale = Grayscale::average;
            sub->add("-g,--grayscale", &grayscale);
            return sub->on_complete
            (
                [&]
                {
                    make_grayscale(&image, grayscale);
                    return argparse::ok;
                }
            );

        }
    );

    filters_subs->add
    (
        "creduce", "Reduce the colors by calculating a new palette and swapping to it",
        [&](argparse::SubParser* sub)
        {
            auto dither = false;
            int depth = 3;
            bool middle_split = false;

            sub->add("--depth", &depth)
                .set_nargs("R")
                .set_help("change the palette depth")
                ;
            sub->set_true("--middle", &middle_split)
                .set_help("split at middle array insteaf of median")
                ;
            sub->set_true("-d, --dither", &dither)
                .set_help("Dither the colors instead of replacing");
                ;

            return sub->on_complete
            (
                [&]
                {
                    auto colors = extract_palette_median_cut(image, depth, middle_split);
                    const auto pal = Palette{"", colors};

                    if(dither)
                    {
                        match_palette_dither(&image, pal);
                    }
                    else
                    {
                        match_palette(&image, pal);
                    }

                    return argparse::ok;
                }
            );
        }
    );

    filters_subs->add
    (
        "palswap", "Switch palette",
        [&](argparse::SubParser* sub)
        {
            auto palette_name = palettes::Name::one_bit;
            auto dither = false;

            sub->add("-p, --palette", &palette_name);
            sub->set_true("-d, --dither", &dither);

            return sub->on_complete
            (
                [&]
                {
                    const auto& palette = palettes::get_palette(palette_name);

                    if(dither)
                    {
                        match_palette_dither(&image, palette);
                    }
                    else
                    {
                        match_palette(&image, palette);
                    }

                    return argparse::ok;
                }
            );
        }
    );

    filters_subs->add
    (
        "edge", "Edge detection",
        [&](argparse::SubParser* sub)
        {
            float edge_r = 0.5f;
            sub->add("-r, --range", &edge_r);
            return sub->on_complete
            (
                [&]
                {
                    edge_detection(&image, edge_r);
                    return argparse::ok;
                }
            );
        }
    );

    filters_subs->add
    (
        "color", "Detect colors",
        [&](argparse::SubParser* sub)
        {
            float edge_r = 0.5f;
            auto color_color = NamedColor::red;
            sub->add("-r, --range", &edge_r);
            sub->add("-c, --color", &color_color);
            return sub->on_complete
            (
                [&]
                {
                    color_detection(&image, color_color, edge_r);
                    return argparse::ok;
                }
            );
        }
    );

    filters_subs->add
    (
        "bright", "Change brightness",
        [&](argparse::SubParser* sub)
        {
            int bright_c = 10;
            sub->add("-c, --change", &bright_c);
            return sub->on_complete
            (
                [&]
                {
                    change_brightness(&image, bright_c);
                    return argparse::ok;
                }
            );
        }
    );

    filters_subs->add
    (
        "contrast", "Change contrast",
        [&](argparse::SubParser* sub)
        {
            float contrast = 10;
            sub->add("-c, --change", &contrast);
            return sub->on_complete
            (
                [&]
                {
                    change_contrast(&image, angle::from_degrees(contrast).get_wrapped());
                    return argparse::ok;
                }
            );
        }
    );

    return argparse::parse_from_main(&parser, argc, argv);
}

