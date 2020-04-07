#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <utility>
#include <optional>

#include "core/argparse.h"
#include "core/io.h"
#include "core/image.h"
#include "core/image_draw.h"
#include "core/pack.h"


using namespace euphoria::core;


std::optional
<
    std::pair
    <
        std::vector<ImageLoadResult>,
        std::vector<Sizei>
    >
>
LoadImages(const std::vector<std::string>& files, int padding)
{
    auto images = std::vector<ImageLoadResult>(files.size());
    auto image_sizes = std::vector<Sizei>(files.size());

    int i = 0;
    for(const auto& f: files)
    {
        const auto index = i; i += 1;

        auto chunk = io::FileToChunk(f);
        if(chunk == nullptr)
        {
            std::cerr << "failed to read " << f << "\n";
            return std::nullopt;
        }
        images[index] = LoadImage(chunk, f, AlphaLoad::Keep);
        if(images[index].error.empty() == false)
        {
            std::cerr << "failed to read image " <<
                images[index].error << "\n";
            return std::nullopt;
        }

        image_sizes[index] = Sizei::FromWidthHeight
        (
            images[index].image.GetWidth() + padding,
            images[index].image.GetHeight() + padding
        );
    }

    return {{images, image_sizes}};
}


std::optional<Recti>
PackTight(const std::vector<std::optional<Recti>>& packed)
{
    std::optional<Recti> bb = std::nullopt;

    for(const auto& rect: packed)
    {
        if(rect.has_value())
        {
            if(bb.has_value())
            {
                bb->Include(*rect);
            }
            else
            {
                bb = *rect;
            }
        }
    }

    return bb;
}


int
CountAndReportFailedPacks
(
    const std::vector<std::optional<Recti>>& packed,
    const std::vector<std::string>& files
)
{
    int failed_pack_count = 0;

    int i = 0;
    for(const auto& rect: packed)
    {
        const auto index = i; i += 1;

        if(rect.has_value() == false)
        {
            std::cerr << "failed to pack " << files[index] << "\n";
            failed_pack_count += 1;
        }
    }

    return failed_pack_count;
}


Image
DrawImage
(
    const std::vector<ImageLoadResult>& images,
    const std::vector<std::optional<Recti>>& packed,
    const Sizei& size,
    int padding,
    const Rgbi& background_color,
    int dx,
    int dy
)
{
    auto composed_image = Image{};
    composed_image.SetupWithAlphaSupport
    (
        size.width + padding,
        size.height + padding
    );
    Clear(&composed_image, background_color);

    int i = 0;
    for(const auto& rect: packed)
    {
        const auto index = i; i += 1;

        if(rect.has_value() == false) { continue; }

        // std::cout << rect->BottomLeft() << "\n";
        PasteImage
        (
            &composed_image,
            rect->BottomLeft() + vec2i{dx, dy} + vec2i{padding, padding},
            images[index].image,
            PixelsOutside::Discard
        );
    }

    return composed_image;
}

bool
HandlePack
(
    const std::string& output_file,
    const Sizei& image_size,
    int padding,
    Rgbi background_color,
    bool pack_image,
    const std::vector<std::string>& files
)
{
    // load images
    const auto loaded_images = LoadImages(files, padding);
    if(loaded_images.has_value() == false)
    {
        return false;
    }
    const auto& [images, image_sizes] = *loaded_images;

    // pack images
    const auto packed = Pack(image_size, image_sizes);
    const int failed_pack_count = CountAndReportFailedPacks(packed, files);

    if(failed_pack_count > 0)
    {
        // return argparse::ParseResult::Error;
    }


    // reduce image size
    const auto
    [
        size,
        dx,
        dy
    ] = [&]() -> std::tuple<Sizei, int, int>
    {
        if(pack_image)
        {
            if(const auto bb = PackTight(packed); bb.has_value())
            {
                return {bb->GetSize(), -bb->left, -bb->bottom};
            }
        }
        return {image_size, 0, 0};
    }();

    // draw new image
    auto composed_image = DrawImage
    (
        images,
        packed,
        size,
        padding,
        background_color,
        dx,
        dy
    );

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
        "pack", "pack images according to the stb rect-pack algorithm",
        [](argparse::SubParser* sub)
        {
            // todo(Gustav): expose as commandline arguments
            auto image_size = Sizei::FromWidthHeight(1014, 1024);
            auto background_color = Color::Gray;

            std::string output_file = "collage.png";
            int padding = 5;
            bool pack_image = true;
            std::vector<std::string> files;

            sub->Add("--padding", &padding)
                .AllowBeforePositionals()
                .Nargs("P").Help("set the space (in pixels) between images")
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
