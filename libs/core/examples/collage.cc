#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <utility>
#include <optional>

#include "core/argparse.h"
#include "core/io.h"
#include "core/image.h"
#include "core/stringutils.h"
#include "core/collage.h"


// todo(Gustav): move all logic to collage.cc/collage.h


using namespace euphoria::core;


std::vector<Image>
load_images(const std::vector<std::string>& files)
{
    auto images = std::vector<Image>{};

    for(const auto& f: files)
    {
        auto chunk = io::file_to_chunk(f);
        if(chunk == nullptr)
        {
            std::cerr << "failed to read " << f << "\n";
            return {};
        }
        auto loaded_image = load_image(chunk, f, AlphaLoad::keep);
        if(loaded_image.error.empty() == false)
        {
            std::cerr << "failed to read image " <<
                loaded_image.error << "\n";
            return {};
        }

        images.emplace_back(loaded_image.image);
    }

    return images;
}


bool
handle_grid
(
    const std::string& output_file,
    int padding,
    Rgbi background_color,
    const std::vector<std::string>& src_files,
    bool top_to_bottom,
    bool sort_files
)
{
    auto files = src_files;

    if(sort_files)
    {
        std::sort
        (
            files.begin(),
            files.end(),
            [](const std::string& lhs, const std::string& rhs)
            {
                return string_compare(lhs, rhs) < 0;
            }
        );
    }

    // load images
    const auto images = load_images(files);
    if(images.empty())
    {
        return false;
    }

    // draw new image
    auto composed_image = grid_layout
    (
        images,
        padding,
        background_color,
        top_to_bottom
    );

    // save image to out
    auto saved_chunk = composed_image.write(ImageWriteFormat::png);
    io::chunk_to_file(saved_chunk, output_file);

    return true;
}


bool
handle_pack
(
    const std::string& output_file,
    const Size2i& requested_size,
    int padding,
    Rgbi background_color,
    bool should_pack_image,
    const std::vector<std::string>& files
)
{
    if( requested_size.width < padding ||
        requested_size.height < padding)
    {
        std::cerr << "padding too large\n";
        return false;
    }

    const auto image_size = Size2i::create_from_width_height
    (
        requested_size.width - padding,
        requested_size.height - padding
    );

    // load images
    const auto images = load_images(files);
    if(images.empty())
    {
        return false;
    }

    // pack images
    auto packed = pack_image
    (
        images,
        files,
        image_size,
        padding,
        background_color,
        should_pack_image
    );

    if(packed.has_value() == false)
    {
        return false;
    }

    // draw new image
    auto composed_image = *packed;

    // save image to out
    auto saved_chunk = composed_image.write(ImageWriteFormat::png);
    io::chunk_to_file(saved_chunk, output_file);

    return true;
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser
    {
        "pack smaller images into a bigger one"
    };
    auto subs = parser.add_sub_parsers();

    subs->add
    (
        "grid", "lay put images in a grid",
        [](argparse::SubParser* sub)
        {
            Rgbi background_color = NamedColor::gray;
            std::string output_file = "collage.png";
            int padding = 5;
            bool top_to_bottom = true;
            std::vector<std::string> files;
            bool sort_files = false;

            sub->add("--bg", &background_color)
                .set_allow_before_positionals()
                .set_nargs("C")
                .set_help("change the background color")
                ;
            sub->add("--padding", &padding)
                .set_allow_before_positionals()
                .set_nargs("P")
                .set_help("change the space (in pixels) between images")
                ;
            sub->add("-o, --output", &output_file)
                .set_allow_before_positionals()
                .set_nargs("FILE")
                .set_help("change where to save the output")
                ;
            sub->add_vector("files", &files)
                .set_nargs("F")
                .set_help("the files to pack")
                ;
            sub->set_false("--invert", &top_to_bottom)
                .set_allow_before_positionals()
                .set_help("switch from top-to-bottom to bottom-to-top layout")
                ;
            sub->set_true("--sort", &sort_files)
                .set_allow_before_positionals()
                .set_help("sort image files before laying them out in the grid")
                ;
            return sub->on_complete([&]
            {
                const auto was_packed = handle_grid
                (
                    output_file,
                    padding,
                    background_color,
                    files,
                    top_to_bottom,
                    sort_files
                );

                return was_packed
                    ? argparse::ok
                    : argparse::error
                    ;
            });
        }
    );

    subs->add
    (
        "pack", "pack images according to the stb rect-pack algorithm",
        [](argparse::SubParser* sub)
        {
            auto image_size = Size2i::create_from_width_height(1024, 1024);
            Rgbi background_color = NamedColor::gray;
            std::string output_file = "collage.png";
            int padding = 5;
            bool pack_image = true;
            std::vector<std::string> files;

            sub->add("--size", &image_size)
                .set_allow_before_positionals()
                .set_nargs("S")
                .set_help("change the image size")
                ;
            sub->add("--bg", &background_color)
                .set_allow_before_positionals()
                .set_nargs("C")
                .set_help("change the background color")
                ;
            sub->add("--padding", &padding)
                .set_allow_before_positionals()
                .set_nargs("P")
                .set_help("change the space (in pixels) between images")
                ;
            sub->add("-o, --output", &output_file)
                .set_allow_before_positionals()
                .set_nargs("FILE")
                .set_help("change where to save the output")
                ;
            sub->set_false("--no-pack", &pack_image)
                .set_allow_before_positionals()
                .set_help("don't pack the resulting image and keep the whitespace")
                ;
            sub->add_vector("files", &files)
                .set_nargs("F")
                .set_help("the files to pack")
                ;

            return sub->on_complete([&]
            {
                const auto was_packed = handle_pack
                (
                    output_file,
                    image_size,
                    padding,
                    background_color,
                    pack_image,
                    files
                );

                return was_packed
                    ? argparse::ok
                    : argparse::error
                    ;
            });
        }
    );

    return parse_from_main(&parser, argc, argv);
}
