#include "core/argparse.h"

#include "core/drunken_bishop.h"

#include "core/random.h"
#include "core/imageops.h"
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


Table<int>
GenerateDrunkenBishopTable(Random* random, const Common& common)
{
    auto hash = std::vector<int>{};
    const int times = common.big ? 8 : 4;
    for(int i=0; i<times; i+=1)
    {
        const auto codes = ToCodes(ToBytes(random->NextInteger()), true);
        for(auto c : codes)
        {
            hash.emplace_back(c);
        }
    }
    return DrunkenBishop(hash, common.width, common.height);
}


image
GenerateImage(const Table<int>& table, int scale, const Palette& pal)
{
    image image;
    image.setup_no_alpha_support(scale * table.GetWidth(), scale * table.GetHeight());

    auto draw_rect = [&](const rgbi& color, const vec2i& top_left)
    {
        DrawRect
        (
            &image,
            color,
            Recti::FromTopLeftWidthHeight
            (
                top_left,
                scale+1, scale
            )
        );
    };

    for(int y=0; y<table.GetHeight(); y+=1)
    {
        for(int x=0; x<table.GetWidth(); x+=1)
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
            draw_rect(pal.colors[v], vec2i{x*scale, (y+1)*scale});
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
            auto pal = palette::PaletteName::Cubehelix1;

            common.Add(sub);
            sub->add("--pal", &pal).set_help("Set the palette");
            sub->add("--count", &count).set_help("The number of images");
            sub->add("--scale", &scale).set_help("The scale of the image");
            return sub->on_complete([&]{
                auto random = Random{};
                for(int c=0; c<count; c+=1)
                {
                    const auto table = GenerateDrunkenBishopTable(&random, common);
                    const auto image = GenerateImage
                    (
                        table,
                        scale,
                        palette::GetPalette(pal)
                    );
                    const std::string file_name = count == 1
                        ? std::string("bishop.png")
                        : (Str() << "bishop_" << (c+1) << ".png")
                        ;
                    io::ChunkToFile(image.write(ImageWriteFormat::PNG), file_name);
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
                auto random = Random{};
                const auto table = GenerateDrunkenBishopTable(&random, common);
                const auto strs = Collapse(table, GetSshCharacters());
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
