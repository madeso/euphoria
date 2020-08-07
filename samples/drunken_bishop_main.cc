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
    Add(argparse::ParserBase* sub)
    {
        sub->Add("--width", &width).Nargs("W").Help("set the height");
        sub->Add("--height", &height).Nargs("H").Help("set the width");
        sub->SetTrue("--256", &big).Help("Upgrade from 128 to 256 bit hash");
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

    auto subs = parser.AddSubParsers();

    subs->Add
    (
        "img", "drunken bishop with img style",
        [](argparse::SubParser* sub)
        {
            auto common = Common{};
            int count = 1;
            int scale = 10;
            auto pal = palette::PaletteName::Cubehelix1;

            common.Add(sub);
            sub->Add("--pal", &pal).Help("Set the palette");
            sub->Add("--count", &count).Help("The number of images");
            sub->Add("--scale", &scale).Help("The scale of the image");
            return sub->OnComplete([&]{
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
                    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), file_name);
                }

                return argparse::ParseResult::Ok;
            });
        }
    );

    subs->Add
    (
        "text",
        "drunken bishop with ssh like output",
        [](argparse::SubParser* sub)
        {
            auto common = Common{};
            common.Add(sub);
            return sub->OnComplete([&]{
                auto random = Random{};
                const auto table = GenerateDrunkenBishopTable(&random, common);
                const auto strs = Collapse(table, GetSshCharacters());
                for(const auto str: strs)
                {
                    std::cout << str << "\n";
                }

                return argparse::ParseResult::Ok;
            });
        }
    );

    return ParseFromMain(&parser, argc, argv);
}
