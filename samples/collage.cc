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
LoadImages(const std::vector<std::string>& files)
{
    auto images = std::vector<Image>{};

    for(const auto& f: files)
    {
        auto chunk = io::FileToChunk(f);
        if(chunk == nullptr)
        {
            std::cerr << "failed to read " << f << "\n";
            return {};
        }
        auto loaded_image = LoadImage(chunk, f, AlphaLoad::Keep);
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
HandleGrid
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
                return StringCompare(lhs, rhs) < 0;
            }
        );
    }

    // load images
    const auto images = LoadImages(files);
    if(images.empty())
    {
        return false;
    }

    // draw new image
    auto composed_image = GridLayout
    (
        images,
        padding,
        background_color,
        top_to_bottom
    );

    // save image to out
    auto saved_chunk = composed_image.Write(ImageWriteFormat::PNG);
    io::ChunkToFile(saved_chunk, output_file);

    return true;
}


bool
HandlePack
(
    const std::string& output_file,
    const Sizei& requested_size,
    int padding,
    Rgbi background_color,
    bool pack_image,
    const std::vector<std::string>& files
)
{
    if( requested_size.width < padding ||
        requested_size.height < padding)
    {
        std::cerr << "padding too large\n";
        return false;
    }

    const auto image_size = Sizei::FromWidthHeight
    (
        requested_size.width - padding,
        requested_size.height - padding
    );

    // load images
    const auto images = LoadImages(files);
    if(images.empty())
    {
        return false;
    }

    // pack images
    auto packed = PackImage
    (
        images,
        files,
        image_size,
        padding,
        background_color,
        pack_image
    );

    if(packed.has_value() == false)
    {
        return false;
    }

    // draw new image
    auto composed_image = *packed;

    // save image to out
    auto saved_chunk = composed_image.Write(ImageWriteFormat::PNG);
    io::ChunkToFile(saved_chunk, output_file);

    return true;
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser
    {
        "pack smaller images into a bigger one"
    };
    auto subs = parser.AddSubParsers();

    subs->Add
    (
        "grid", "lay put images in a grid",
        [](argparse::SubParser* sub)
        {
            Rgbi background_color = Color::Gray;
            std::string output_file = "collage.png";
            int padding = 5;
            bool top_to_bottom = true;
            std::vector<std::string> files;
            bool sort_files = false;

            sub->Add("--bg", &background_color)
                .AllowBeforePositionals()
                .Nargs("C")
                .Help("change the background color")
                ;
            sub->Add("--padding", &padding)
                .AllowBeforePositionals()
                .Nargs("P")
                .Help("change the space (in pixels) between images")
                ;
            sub->Add("-o, --output", &output_file)
                .AllowBeforePositionals()
                .Nargs("FILE")
                .Help("change where to save the output")
                ;
            sub->AddVector("files", &files)
                .Nargs("F")
                .Help("the files to pack")
                ;
            sub->SetFalse("--invert", &top_to_bottom)
                .AllowBeforePositionals()
                .Help("switch from top-to-bottom to bottom-to-top layout")
                ;
            sub->SetTrue("--sort", &sort_files)
                .AllowBeforePositionals()
                .Help("sort image files before laying them out in the grid")
                ;
            return sub->OnComplete([&]
            {
                const auto was_packed = HandleGrid
                (
                    output_file,
                    padding,
                    background_color,
                    files,
                    top_to_bottom,
                    sort_files
                );

                return was_packed
                    ? argparse::ParseResult::Ok
                    : argparse::ParseResult::Error
                    ;
            });
        }
    );

    subs->Add
    (
        "pack", "pack images according to the stb rect-pack algorithm",
        [](argparse::SubParser* sub)
        {
            auto image_size = Sizei::FromWidthHeight(1024, 1024);
            Rgbi background_color = Color::Gray;
            std::string output_file = "collage.png";
            int padding = 5;
            bool pack_image = true;
            std::vector<std::string> files;

            sub->Add("--size", &image_size)
                .AllowBeforePositionals()
                .Nargs("S")
                .Help("change the image size")
                ;
            sub->Add("--bg", &background_color)
                .AllowBeforePositionals()
                .Nargs("C")
                .Help("change the background color")
                ;
            sub->Add("--padding", &padding)
                .AllowBeforePositionals()
                .Nargs("P")
                .Help("change the space (in pixels) between images")
                ;
            sub->Add("-o, --output", &output_file)
                .AllowBeforePositionals()
                .Nargs("FILE")
                .Help("change where to save the output")
                ;
            sub->SetFalse("--no-pack", &pack_image)
                .AllowBeforePositionals()
                .Help("don't pack the resulting image and keep the whitespace")
                ;
            sub->AddVector("files", &files)
                .Nargs("F")
                .Help("the files to pack")
                ;

            return sub->OnComplete([&]
            {
                const auto was_packed = HandlePack
                (
                    output_file,
                    image_size,
                    padding,
                    background_color,
                    pack_image,
                    files
                );

                return was_packed
                    ? argparse::ParseResult::Ok
                    : argparse::ParseResult::Error
                    ;
            });
        }
    );

    return ParseFromMain(&parser, argc, argv);
}
