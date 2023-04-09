#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/cint.h"
#include "core/argparse.h"
#include "core/palette_all.h"
#include "core/palette.h"
#include "core/dump.h"
#include "core/os.h"


#include <fstream>


using namespace euphoria::core;


void
print_palette_names()
{
    for (auto palette_name : palettes::palette_names)
    {
        const auto pal = palettes::get_palette(palette_name);
        std::cout << pal.name << "\n";
    }
}


void
write_palettes_to_files(int image_size)
{
    for (auto palette_name : palettes::palette_names)
    {
        const auto pal = palettes::get_palette(palette_name);

        Image image;
        image.setup_no_alpha_support(image_size * c_sizet_to_int(pal.colors.size()), image_size);

        for
        (
            int color_index = 0;
            color_index < c_sizet_to_int(pal.colors.size());
            color_index += 1
        )
        {
            draw_square
            (
                &image,
                pal.colors[color_index],
                image_size * color_index,
                image_size - 1,
                image_size
            );
        }

        const std::string file = fmt::format("palette_{}.png", pal.name);
        
        io::write_chunk_to_file(image.write(ImageWriteFormat::png), file);
    }
}



void
write_palettes_to_html(const std::string& wd)
{
    const std::string file = fmt::format("{}/palettes.html", wd);
    std::ofstream handle;
    handle.open(file.c_str());
    handle << R"(
    <!DOCTYPE html>
    <html lang="en-US">
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta name="referrer" content="no-referrer">
        <link rel="stylesheet" type="text/css" media="screen" href="style.css"/>
        <title>Palettes</title>
            <style>
                p.pal
                {
                    background: #aaa;
                    padding: 6px;
                    display: inline-block;
                }
                span.color
                {
                    float: left;
                    width:32px;
                    height:32px;
                }
            </style>
        </head>
        <body>
        <div id="root">
            <div id="page">
                <div id="content">
                    <h1>Palettes</h1>
    )";

    for (auto palette_name : palettes::palette_names)
    {
        const auto pal = palettes::get_palette(palette_name);

        handle << "<h2>" << pal.name << "</h2>\n";
        handle << "<p class=\"pal\">\n";

        for
        (
            int color_index = 0;
            color_index < c_sizet_to_int(pal.colors.size());
            color_index += 1
        )
        {
            const auto c = pal.colors[color_index];
            handle
                << "<span "
                << "class=\"color\""
                << "style=\"background:rgb("
                    << static_cast<int>(c.r) << ", "
                    << static_cast<int>(c.g) << ", "
                    << static_cast<int>(c.b) << ");\""
                <<">&nbsp</span>\n";
        }

        handle << "</p>\n";
    }

    handle << R"(
                </div>
            </div>
        </div>
    </body>
</html>
    )";
}



dump2d::Poly create_box(const vec2f& p, float width, float height, const Rgbi& c, bool border)
{
    dump2d::Poly r;
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
    auto d = dump2d::Dumper{};

    constexpr float size = 10.0f;
    constexpr float spacing = 1.0f;

    constexpr float ss = size + spacing;

    constexpr float x = 0.0f;
    float y = 0;

    for (auto palette_name : palettes::palette_names)
    {
        const auto pal = palettes::get_palette(palette_name);

        for
        (
            int color_index = 0;
            color_index < c_sizet_to_int(pal.colors.size());
            color_index += 1
        )
        {
            const auto s = border ? ss : size-spacing*2;
            const auto box_position = vec2f{x + s * euphoria::core::c_int_to_float(color_index), y};
            d << create_box(box_position, size, height, pal.colors[color_index], border);
        }

        y += height + spacing;
    }

    d.write(path);
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser {"euphoria palette generator"};

    auto filters_subs = parser.add_sub_parsers();

    filters_subs->add
    (
        "write-many", "write each palette to a image file",
        [&](argparse::SubParser* sub)
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
        "write-html", "write each palette to a image file",
        [&](argparse::SubParser* sub)
        {
            auto current_directory = get_current_directory();
            sub->add("-w", &current_directory);

            return sub->on_complete
            (
                [&]
                {
                    write_palettes_to_html(current_directory);
                    return argparse::ok;
                }
            );
        }
    );

    filters_subs->add
    (
        "write-single", "write all palettes to a single svg dump",
        [&](argparse::SubParser* sub)
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
        [&](argparse::SubParser* sub)
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

