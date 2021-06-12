#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/cint.h"
#include "core/argparse.h"
#include "core/palette_all.h"
#include "core/palette.h"
#include "core/str.h"
#include "core/cint.h"
#include "core/dump.h"


using namespace euphoria::core;


void
print_palette_names()
{
    for (auto palette_name : palettes::PaletteNames)
    {
        const auto pal = palettes::get_palette(palette_name);
        std::cout << pal.name << "\n";
    }
}


void
write_palettes_to_files(int image_size)
{
    for (auto palette_name : palettes::PaletteNames)
    {
        const auto pal = palettes::get_palette(palette_name);

        image image;
        image.setup_no_alpha_support(image_size * Csizet_to_int(pal.colors.size()), image_size);

        for
        (
            int i = 0;
            i < Csizet_to_int(pal.colors.size());
            i += 1
        )
        {
            draw_square
            (
                &image,
                pal.colors[i],
                image_size * i,
                image_size - 1,
                image_size
            );
        }

        const std::string file = string_builder{} << "palette_" << pal.name << ".png";

        io::chunk_to_file(image.write(image_write_format::png), file);
    }
}

dump2d::poly create_box(const vec2f& p, float width, float height, const rgbi& c, bool border)
{
    dump2d::poly r;
    r.points =
    {
        p,
        p + vec2f{width, 0},
        p + vec2f{width, height},
        p + vec2f{0, height}
    };
    r.close();
    r.fill(c);
    if(border == false)
    {
        r.stroke_width = -1.0f;
    }
    return r;
}

void
write_palettes_to_single_svg(const std::string& path, float height, bool border)
{
    auto d = dump2d::dumper{};

    constexpr float size = 10.0f;
    constexpr float spacing = 1.0f;

    constexpr float ss = size + spacing;

    constexpr float x = 0.0f;
    float y = 0;

    for (auto palette_name : palettes::PaletteNames)
    {
        const auto pal = palettes::get_palette(palette_name);

        for
        (
            int i = 0;
            i < Csizet_to_int(pal.colors.size());
            i += 1
        )
        {
            const auto s = border ? ss : size-spacing*2;
            d << create_box(vec2f{x + s * i, y}, size, height, pal.colors[i], border);
        }

        y += height + spacing;
    }

    d.write(path);
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::parser {"euphoria palette generator"};

    auto filters_subs = parser.add_sub_parsers();

    filters_subs->add
    (
        "write-many", "write each palette to a image file",
        [&](argparse::sub_parser* sub)
        {
            auto image_size = 32;
            sub->add("--size", &image_size).set_help("Size of the color block");

            return sub->on_complete
            (
                [&]
                {
                    write_palettes_to_files(image_size);
                    return argparse::ok;
                }
            );
        }
    );

    filters_subs->add
    (
        "write-single", "write all palettes to a single html dump",
        [&](argparse::sub_parser* sub)
        {
            bool add_border = false;
            float height = 10.0f;
            std::string path = "palettes.html";
            sub->add("--path", &path).set_help("Where to write the html dump");
            sub->add("--height", &height).set_help("the height of the boxes");
            sub->set_true("--border", &add_border);

            return sub->on_complete
            (
                [&]
                {
                    write_palettes_to_single_svg(path, height, add_border);
                    return argparse::ok;
                }
            );
        }
    );

    filters_subs->add
    (
        "ls", "print all palette names",
        [&](argparse::sub_parser* sub)
        {
            return sub->on_complete
            (
                [&]
                {
                    print_palette_names();
                    return argparse::ok;
                }
            );
        }
    );

    return argparse::parse_from_main(&parser, argc, argv);
}

