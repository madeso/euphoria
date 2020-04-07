#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "core/argparse.h"

#include "core/io.h"
#include "core/image.h"
#include "core/image_draw.h"
#include "core/pack.h"


using namespace euphoria::core;


int
main(int argc, char* argv[])
{
    // todo(Gustav): expose as commandline arguments
    std::string output_file = "collage.png";
    auto image_size = Sizei::FromWidthHeight(1014, 1024);
    int padding = 5;
    auto background_color = Color::Gray;
    bool pack_image = true;

    std::vector<std::string> files;

    auto parser = argparse::Parser {"collage tool"};

    parser.Add("-o, --output", &output_file);
    parser.AddVector("files", &files).Nargs("F");
    parser.OnComplete([&]
    {
        auto images = std::vector<ImageLoadResult>(files.size());
        auto image_sizes = std::vector<Sizei>(files.size());
        int i = 0;

        // load images
        for(const auto& f: files)
        {
            const auto index = i; i += 1;

            auto chunk = io::FileToChunk(f);
            if(chunk == nullptr)
            {
                std::cerr << "failed to read " << f << "\n";
                return argparse::ParseResult::Error;
            }
            images[index] = LoadImage(chunk, f, AlphaLoad::Keep);
            if(images[index].error.empty() == false)
            {
                std::cerr << "failed to read image " <<
                    images[index].error << "\n";
                return argparse::ParseResult::Error;
            }

            image_sizes[index] = Sizei::FromWidthHeight
            (
                images[index].image.GetWidth() + padding,
                images[index].image.GetHeight() + padding
            );
        }

        // pack images
        const auto packed = Pack(image_size, image_sizes);
        bool all_packed = true; // updated when drawing

        // reduce image size
        auto size = image_size;
        int dx = 0;
        int dy = 0;
        if(pack_image)
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
            if(bb.has_value())
            {
                size = bb->GetSize();
                dx = -bb->left;
                dy = -bb->bottom;
            }
        }

        // draw new image
        // todo(Gustav): add option to pack make image smaller than requested
        // to reduce wasted space and tighly pack the images
        auto composed_image = Image{};
        composed_image.SetupWithAlphaSupport
        (
            size.width + padding,
            size.height + padding
        );
        Clear(&composed_image, background_color);
        i = 0;
        for(const auto& rect: packed)
        {
            const auto index = i; i += 1;

            if(rect.has_value() == false)
            {
                std::cerr << "failed to pack " << files[i] << "\n";
                all_packed = false;
                continue;
            }

            // std::cout << rect->BottomLeft() << "\n";
            PasteImage
            (
                &composed_image,
                rect->BottomLeft() + vec2i{dx, dy} + vec2i{padding, padding},
                images[index].image,
                PixelsOutside::Discard
            );
        }

        if(all_packed == false)
        {
            // return argparse::ParseResult::Error;
        }


        // save image to out
        auto saved_chunk = composed_image.Write(ImageWriteFormat::PNG);
        io::ChunkToFile(saved_chunk, output_file);
        return argparse::ParseResult::Ok;
    });

    return ParseFromMain(&parser, argc, argv);
}
