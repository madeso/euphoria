#include "core/argparse.h"

#include "core/drunken_bishop.h"

#include "core/random.h"
#include "core/image_to_text.h"
#include "core/palette.h"
#include "core/palette_tableu.h"
#include "core/palette_all.h"
#include "core/image.h"
#include "core/image_draw.h"
#include "core/str.h"

#include "core/io.h"

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace euphoria::core;


struct Common
{
    int width = 17;
    int height = 9;
    bool big = false; // 256 or 128 bit

    void
    Add(argparse::parser_base* sub)
    {
        sub->add("--width", &width).set_nargs("W").set_help("set the height");
        sub->add("--height", &height).set_nargs("H").set_help("set the width");
        sub->set_true("--256", &big).set_help("Upgrade from 128 to 256 bit hash");
    }
};


table<int>
GenerateDrunkenBishopTable(random* random, const Common& common)
{
    auto hash = std::vector<int>{};
    const int times = common.big ? 8 : 4;
    for(int i=0; i<times; i+=1)
    {
        const auto codes = to_codes(to_bytes(random->get_next_integer32()), true);
        for(auto c : codes)
        {
            hash.emplace_back(c);
        }
    }
    return drunken_bishop(hash, common.width, common.height);
}


image
GenerateImage(const table<int>& table, int scale, const palette& pal)
{
    image image;
    image.setup_no_alpha_support(scale * table.get_width(), scale * table.get_height());

    auto rect = [&](const rgbi& color, const vec2i& top_left)
    {
        draw_rect
        (
            &image,
            color,
            recti::from_top_left_width_height
            (
                top_left,
                scale+1, scale
            )
        );
    };

    for(int y=0; y<table.get_height(); y+=1)
    {
        for(int x=0; x<table.get_width(); x+=1)
        {
            const auto v =
            std::max
            (
                0,
                std::min
                (
                    table(x,y),
                    Csizet_to_int(pal.colors.size())
                )
            );
            rect(pal.colors[v], vec2i{x*scale, (y+1)*scale});
        }
    }

    return image;
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::parser {"Drunken bishops"};

    auto subs = parser.add_sub_parsers();

    subs->add
    (
        "img", "drunken bishop with img style",
        [](argparse::sub_parser* sub)
        {
            auto common = Common{};
            int count = 1;
            int scale = 10;
            auto pal = palettes::name::cubehelix_1;

            common.Add(sub);
            sub->add("--pal", &pal).set_help("Set the palette");
            sub->add("--count", &count).set_help("The number of images");
            sub->add("--scale", &scale).set_help("The scale of the image");
            return sub->on_complete([&]{
                auto rand = random{};
                for(int c=0; c<count; c+=1)
                {
                    const auto table = GenerateDrunkenBishopTable(&rand, common);
                    const auto image = GenerateImage
                    (
                        table,
                        scale,
                        palettes::get_palette(pal)
                    );
                    const std::string file_name = count == 1
                        ? std::string("bishop.png")
                        : (string_builder() << "bishop_" << (c+1) << ".png")
                        ;
                    io::chunk_to_file(image.write(image_write_format::png), file_name);
                }

                return argparse::ok;
            });
        }
    );

    subs->add
    (
        "text",
        "drunken bishop with ssh like output",
        [](argparse::sub_parser* sub)
        {
            auto common = Common{};
            common.Add(sub);
            return sub->on_complete([&]{
                auto rand = random{};
                const auto table = GenerateDrunkenBishopTable(&rand, common);
                const auto strs = collapse(table, get_ssh_characters());
                for(const auto& str: strs)
                {
                    std::cout << str << "\n";
                }

                return argparse::ok;
            });
        }
    );

    return parse_from_main(&parser, argc, argv);
}
