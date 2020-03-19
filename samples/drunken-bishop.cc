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


Table<int>
GenerateDrunkenBishopTable(Random* random, int width, int height, bool big)
{
    auto hash = std::vector<int>{};
    const int times = big ? 8 : 4;
    for(int i=0; i<times; i+=1)
    {
        const auto codes = ToCodes(ToBytes(random->NextInteger()), true);
        for(auto c : codes)
        {
            hash.emplace_back(c);
        }
    }
    return DrunkenBishop(hash, width, height);
}


Image
GenerateImage(const Table<int>& table, int scale, const Palette& pal)
{
    Image image;
    image.SetupNoAlphaSupport(scale * table.GetWidth(), scale * table.GetHeight());

    auto draw_rect = [&](const Rgbi& color, const vec2i& top_left)
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
    auto parser = argparse::Parser {"Drunken bishops"};

    int width = 17;
    int height = 9;

    int count = 1;
    int scale = 10;
    bool big = false; // 256 or 128 bit
    auto pal = palette::PaletteName::Cubehelix1;

    auto add_common = [&](argparse::Parser& parser)
    {
        parser.AddSimple("--width", &width).Help("set the height");
        parser.AddSimple("--height", &height).Help("set the width");
    };

    auto pimg = parser.AddSubParser
    (
        "img",
        "drunken bishop with img style",
        [&]
        {
            auto random = Random{};
            for(int c=0; c<count; c+=1)
            {
                const auto table = GenerateDrunkenBishopTable(&random, width, height, big);
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
                io::ChunkToFile(image.Write(ImageWriteFormat::PNG), file_name);
            }
        }
    );
    add_common(*pimg);
    pimg->AddEnum("--pal", &pal).Help("Set the palette");
    pimg->SetTrue("--256", &big).Help("Upgrade from 128 to 256 bit hash");
    pimg->AddSimple("--count", &count).Help("The number of images");
    pimg->AddSimple("--scale", &scale).Help("The scale of the image");

    auto precursive = parser.AddSubParser
    (
        "text",
        "drunken bishop with ssh like output",
        [&]
        {
            auto random = Random{};
            const auto table = GenerateDrunkenBishopTable(&random, width, height, big);
            const auto strs = Collapse(table, GetSshCharacters());
            for(const auto str: strs)
            {
                std::cout << str << "\n";
            }
        }
    );
    add_common(*precursive);

    const auto status = parser.Parse(argc, argv);
    if(status != argparse::ParseResult::Ok)
    {
        return -1;
    }

    return 0;
}
