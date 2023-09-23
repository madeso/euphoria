#include <fstream>


#include <iostream>
#include <utility>
#include <optional>
#include <map>

#include "core/argparse.h"
#include "io/io.h"
#include "core/image.h"
#include "base/cint.h"
#include "core/image_draw.h"
#include "core/timepoint.h"
#include "core/palette_extract.h"


using namespace eu::core;
using namespace eu::io;


struct ImageAndFile
{
    ImageAndFile() = default;

    ImageAndFile(const std::string& f, const Image& i)
        : file(f)
        , image(i)
    {}

    std::string file;
    ::Image image;
};


std::vector<ImageAndFile>
load_images(const std::vector<std::string>& files)
{
    auto images = std::vector<ImageAndFile>{};

    for(const auto& f: files)
    {
        auto chunk = read_file_to_chunk(f);
        if(chunk == nullptr)
        {
            std::cerr << "failed to read image file " << f << "\n";
            return {};
        }
        auto loaded_image = load_image(chunk, f, AlphaLoad::keep);
        if(loaded_image.error.empty() == false)
        {
            std::cerr << "failed to read image data " <<
                loaded_image.error << "\n";
            return {};
        }

        images.emplace_back(f, loaded_image.image);
    }

    return images;
}


struct ExtractedColor
{
    ExtractedColor(const Rgbi& r, int c)
        : color(r)
        , count(c)
    {
    }

    Rgbi color;
    int count;
};


int
find(std::vector<ExtractedColor>* psource, const Rgbi& color, float length)
{
    auto& source = *psource;

    for(int index=0; index < eu::c_sizet_to_int(source.size()); index += 1)
    {
        if((to_rgb(source[index].color) - to_rgb(color)).get_length()*255 < length)
        {
            return index;
        }
    }

    source.emplace_back(color, 0);
    return eu::c_sizet_to_int(source.size()) - 1;
}

std::vector<ExtractedColor>
extract_colors(const std::vector<ImageAndFile>& images, float range)
{
    auto ret = std::vector<ExtractedColor>{};

    for(const auto& img: images)
    {
        for(int y=0; y<img.image.height; y+=1)
        {
            for(int x=0; x<img.image.width; x+=1)
            {
                const auto index = find(&ret, to_rgbi(img.image.get_pixel(x,y)), range);
                ret[index].count += 1;
            }
        }
    }

    return ret;
}


std::vector<ExtractedColor>
extract_colors(const std::vector<ImageAndFile>& images)
{
    std::map<int, int> colors;
    for(const auto& img: images)
    {
        for(int y=0; y<img.image.height; y+=1)
        {
            for(int x=0; x<img.image.width; x+=1)
            {
                const auto color = to_rgbi(img.image.get_pixel(x, y));
                const auto hex = color.to_hex();
                auto val = std::get<0>(colors.try_emplace(hex, 0));
                val->second += 1;
            }
        }
    }

    auto ret = std::vector<ExtractedColor>{};
    for(const auto& c: colors)
    {
        ret.emplace_back(Rgbi::from_hex(c.first), c.second);
    }
    return ret;
}


bool
handle_image
(
    const std::vector<std::string>& files,
    int depth,
    bool middle_split,
    int image_size,
    const std::string& file
)
{
    // load images
    const auto images = load_images(files);
    if(images.size() != 1)
    {
        std::cerr << "currently only one image supported...\n";
        return false;
    }

    const auto start = get_current_timepoint();

    // extract colors
    auto colors = extract_palette_median_cut(images[0].image, depth, middle_split);

    const auto end = get_current_timepoint();

    const auto seconds = get_seconds_between(start, end);
    std::cout << "Palette extraction took " << seconds << " seconds.\n";

    if(colors.empty())
    {
        return false;
    }

    Image image;
    image.setup_no_alpha_support(image_size * eu::c_sizet_to_int(colors.size()), image_size);
    for
    (
        int color_index = 0;
        color_index < eu::c_sizet_to_int(colors.size());
        color_index += 1
    )
    {
        draw_square
        (
            &image,
            colors[color_index],
            image_size * color_index,
            image_size - color_index,
            image_size
        );
    }

    write_chunk_to_file(image.write(ImageWriteFormat::png), file);
    return true;
}


bool
handle_print
(
    const std::vector<std::string>& files,
    int depth,
    bool middle_split
)
{
    // load images
    const auto images = load_images(files);
    if(images.size() != 1)
    {
        std::cerr << "currently only one image supported...\n";
        return false;
    }

    // extract colors
    auto colors = extract_palette_median_cut(images[0].image, depth, middle_split);

    std::sort
    (
        colors.begin(),
        colors.end(),
        [](const Rgbi& lhs, const Rgbi& rhs) -> bool
        {
            return to_rgb(lhs).calc_luminance() < to_rgb(rhs).calc_luminance();
        }
    );

    if(colors.empty())
    {
        return false;
    }

    // print colors
    for(auto c: colors)
    {
        fmt::print("{}", c);
    }

    fmt::print("Found #colors: {}", colors.size());
    return true;
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser
    {
        "extract colors from images"
    };
    auto subs = parser.add_sub_parsers();

    subs->add
    (
        "print", "print all colors found in images",
        [](argparse::SubParser* sub)
        {
            std::vector<std::string> files;
            int depth = 3;
            bool middle_split = false;

            sub->add("--depth", &depth)
                .set_allow_before_positionals()
                .set_nargs("R")
                .set_help("change the palette depth")
                ;
            sub->set_true("--middle", &middle_split)
                .set_allow_before_positionals()
                .set_help("split at middle array insteaf of median")
                ;
            sub->add_vector("files", &files)
                .set_nargs("F")
                .set_help("the files to analyze")
                ;

            return sub->on_complete([&]
            {
                const auto was_extracted = handle_print
                (
                    files, depth, middle_split
                );

                return was_extracted
                    ? argparse::ok
                    : argparse::error
                    ;
            });
        }
    );


    subs->add
    (
        "image", "write all colors found in images to a palette image",
        [](argparse::SubParser* sub)
        {
            int image_size = 5;
            std::string file = "pal.png";
            std::vector<std::string> files;
            int depth = 3;
            bool middle_split = false;

            sub->add("--depth", &depth)
                .set_allow_before_positionals()
                .set_nargs("R")
                .set_help("change the palette depth")
                ;
            sub->set_true("--middle", &middle_split)
                .set_allow_before_positionals()
                .set_help("split at middle array insteaf of median")
                ;
            sub->add("--size", &image_size)
                .set_allow_before_positionals()
                .set_nargs("S")
                .set_help("change the image size")
                ;
            sub->add("-o, --output", &file)
                .set_allow_before_positionals()
                .set_nargs("FILE")
                .set_help("change where to save the output")
                ;
            sub->add_vector("files", &files)
                .set_nargs("F")
                .set_help("the files to analyze")
                ;

            return sub->on_complete([&]
            {
                const auto was_extracted = handle_image
                (
                    files,
                    depth,
                    middle_split,
                    image_size,
                    file
                );

                return was_extracted
                    ? argparse::ok
                    : argparse::error
                    ;
            });
        }
    );


    return parse_from_main(&parser, argc, argv);
}
