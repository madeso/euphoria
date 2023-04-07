#include "core/argparse.h"

#include "core/drunken_bishop.h"

#include "core/random.h"
#include "core/image_to_text.h"
#include "core/palette.h"
#include "core/palette_tableu.h"
#include "core/palette_all.h"
#include "core/image.h"
#include "core/image_draw.h"

#include "core/io.h"

#include <iostream>

#include <iomanip>


using namespace euphoria;
using namespace euphoria::core;


struct CommonArguments
{
    int width = 17;
    int height = 9;
    bool big = false; // 256 or 128 bit

    void
    add(argparse::ParserBase* sub)
    {
        sub->add("--width", &width).set_nargs("W").set_help("set the height");
        sub->add("--height", &height).set_nargs("H").set_help("set the width");
        sub->set_true("--256", &big).set_help("Upgrade from 128 to 256 bit hash");
    }
};


Table<int>
generate_drunken_bishop_table(core::Random* random, const ::CommonArguments& common)
{
    auto hash = std::vector<int>{};
    const int times = common.big ? 8 : 4;
    for(int turn=0; turn<times; turn += 1)
    {
        const auto codes = to_codes(to_bytes(random->get_next_u32()), true);
        for(auto c : codes)
        {
            hash.emplace_back(c);
        }
    }
    return get_drunken_bishop_result(hash, common.width, common.height);
}


Image
generate_image(const Table<int>& table, int scale, const Palette& pal)
{
    Image image;
    image.setup_no_alpha_support(scale * table.get_width(), scale * table.get_height());

    auto rect = [&](const rgbi& color, const vec2i& top_left)
    {
        draw_rect
        (
            &image,
            color,
            Recti::from_top_left_width_height
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
                        c_sizet_to_int(pal.colors.size())
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
    auto parser = argparse::Parser {"Drunken bishops"};

    auto subs = parser.add_sub_parsers();

    subs->add
    (
        "img", "drunken bishop with img style",
        [](argparse::SubParser* sub)
        {
            auto common = ::CommonArguments{};
            int count = 1;
            int scale = 10;
            auto pal = palettes::Name::cubehelix_1;

            common.add(sub);
            sub->add("--pal", &pal).set_help("Set the palette");
            sub->add("--count", &count).set_help("The number of images");
            sub->add("--scale", &scale).set_help("The scale of the image");
            return sub->on_complete([&]{
                auto rand = core::Random{};
                for(int turn_index=0; turn_index<count; turn_index+=1)
                {
                    const auto table = generate_drunken_bishop_table(&rand, common);
                    const auto image = generate_image
                    (
                        table,
                        scale,
                        palettes::get_palette(pal)
                    );
                    const std::string file_name = count == 1
                        ? std::string("bishop.png")
                        : fmt::format("bishop_{}.png", turn_index+1)
                        ;
                    io::write_chunk_to_file(image.write(ImageWriteFormat::png), file_name);
                }

                return argparse::ok;
            });
        }
    );

    subs->add
    (
        "text",
        "drunken bishop with ssh like output",
        [](argparse::SubParser* sub)
        {
            auto common = ::CommonArguments{};
            common.add(sub);
            return sub->on_complete([&]{
                auto rand = core::Random{};
                const auto table = generate_drunken_bishop_table(&rand, common);
                const auto strs = render_table(table, get_ssh_characters());
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
