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


struct ImageAndFile
{
    ImageAndFile() {}

    ImageAndFile(const std::string& f, const Image& i)
        : file(f)
        , image(i)
    {}

    std::string file;
    Image image;
};


std::vector<Sizei> CollectSizes
(
    const std::vector<ImageAndFile>& images,
    int padding
)
{
    auto sizes = std::vector<Sizei>{};

    for(const auto& img: images)
    {
        sizes.emplace_back(Sizei::FromWidthHeight
        (
            img.image.GetWidth() + padding,
            img.image.GetHeight() + padding
        ));
    }

    return sizes;
}


std::vector<ImageAndFile>
LoadImages(const std::vector<std::string>& files)
{
    auto images = std::vector<ImageAndFile>{};

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

        images.emplace_back(f, loaded_image.image);
    }

    return images;
}



struct PackedImage
{
    PackedImage()
        : rect(Recti::FromWidthHeight(0,0))
    {}

    PackedImage(const std::string& f, const Image& i, const Recti& p)
        : file(f)
        , image(i)
        , rect(p)
    {}

    std::string file;
    Image image;
    Recti rect;
};


std::vector<PackedImage>
PackImage
(
    const Sizei& image_size,
    const std::vector<ImageAndFile>& images,
    int padding
)
{
    const auto image_sizes = CollectSizes(images, padding);
    const auto packed = Pack(image_size, image_sizes);

    auto ret = std::vector<PackedImage>{};

    int i = 0;
    for(const auto& rect: packed)
    {
        const auto& src = images[i];
        i += 1;

        if(rect.has_value() == false)
        {
            std::cerr << "failed to pack " << src.file << "\n";
        }
        else
        {
            ret.emplace_back(src.file, src.image, *rect);
        }
    }

    return ret;
}


Sizei
PackTight
(
    const Sizei& default_size,
    std::vector<PackedImage>* images
)
{
    std::optional<Recti> bb = std::nullopt;

    for(const auto& img: *images)
    {
        const auto& rect = img.rect;
        if(bb.has_value())
        { bb->Include(rect); }
        else
        { bb = rect; }
    }

    if(!bb)
    {
        return default_size;
    }

    const auto size = bb->GetSize();
    const auto dx = -bb->left;
    const auto dy = -bb->bottom;

    // todo: resize
    for(auto& img: *images)
    {
        img.rect.Offset(dx, dy);
    }

    return size;
}


Image
DrawImage
(
    const std::vector<PackedImage>& images,
    const Sizei& size,
    int padding,
    const Rgbi& background_color
)
{
    auto composed_image = Image{};
    composed_image.SetupWithAlphaSupport
    (
        size.width + padding,
        size.height + padding
    );
    Clear(&composed_image, background_color);

    for(const auto& image: images)
    {
        PasteImage
        (
            &composed_image,
            image.rect.BottomLeft() + vec2i{padding, padding},
            image.image,
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
    const auto images = LoadImages(files);
    if(images.empty())
    {
        return false;
    }

    // pack images
    auto packed = PackImage(image_size, images, padding);

    if(packed.empty())
    {
        return false;
    }

    // optionally reduce image size
    const auto size = pack_image
        ? PackTight(image_size, &packed)
        : image_size
        ;

    // draw new image
    auto composed_image = DrawImage
    (
        packed,
        size,
        padding,
        background_color
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

            // bug: padding is incresed on image
            // if the output isn't packed tight the image is 'padding'
            // larger than requested
            // todo(Gustav): fix bug

            std::string output_file = "collage.png";
            int padding = 5;
            bool pack_image = true;
            std::vector<std::string> files;

            sub->Add("--padding", &padding)
                .AllowBeforePositionals()
                .Nargs("P")
                .Help("set the space (in pixels) between images")
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
