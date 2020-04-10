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
        : position{0,0}
    {}

    PackedImage(const std::string& f, const Image& i, const vec2i& p)
        : file(f)
        , image(i)
        , position(p)
    {}

    std::string file;
    Image image;
    vec2i position;
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
            ret.emplace_back(src.file, src.image, rect->BottomLeft());
        }
    }

    return ret;
}


Sizei
PackTight
(
    const Sizei& default_size,
    std::vector<PackedImage>* images,
    int padding
)
{
    std::optional<Recti> bb = std::nullopt;

    for(const auto& img: *images)
    {
        const auto image_width = img.image.GetWidth();
        const auto image_height = img.image.GetHeight();
        const auto& rect = Recti::FromBottomLeftWidthHeight
        (
            vec2i(img.position.x, img.position.y),
            image_width + padding,
            image_height + padding
        );
        if(bb.has_value())
        { bb->Include(rect); }
        else
        { bb = rect; }
    }
    if(!bb) { return default_size; }

    const auto size = bb->GetSize();
    const auto dx = -bb->left;
    const auto dy = -bb->bottom;

    for(auto& img: *images)
    {
        img.position += vec2i(dx, dy);
    }

    return Sizei::FromWidthHeight(size.width + padding, size.height+padding);
}


Image
DrawImage
(
    const std::vector<PackedImage>& images,
    const Sizei& size,
    const Rgbi& background_color
)
{
    auto composed_image = Image{};
    composed_image.SetupWithAlphaSupport
    (
        size.width,
        size.height
    );
    Clear(&composed_image, background_color);

    for(const auto& image: images)
    {
        PasteImage
        (
            &composed_image,
            image.position,
            image.image,
            PixelsOutside::Discard
        );
    }

    return composed_image;
}


std::pair<std::vector<PackedImage>, Sizei>
GridLayout(const std::vector<ImageAndFile>& images, int padding)
{
    const auto images_per_row = Ceil(Sqrt(images.size()));

    auto ret = std::vector<PackedImage>{};

    int x = padding;
    int y = padding;
    int current_height = 0;
    int column = 0;
    int max_x = 0;

    auto new_row = [&]
    {
        max_x = Max(max_x, x);
        column = 0;
        x = padding;
        y += current_height + padding;
        current_height = 0;
    };

    for(const auto& src: images)
    {
        const auto width = src.image.GetWidth();
        const auto height = src.image.GetHeight();

        ret.emplace_back
        (
            src.file,
            src.image,
            vec2i(x, y)
        );

        x += width + padding;
        current_height = Max(current_height, height);
        column += 1;

        if(column >= images_per_row)
        {
            new_row();
        }
    }

    if(column != 0)
    {
        new_row();
    }

    return {ret, Sizei::FromWidthHeight(max_x, y)};
}


bool
HandleGrid
(
    const std::string& output_file,
    int padding,
    Rgbi background_color,
    const std::vector<std::string>& files
)
{
    // load images
    const auto images = LoadImages(files);
    if(images.empty())
    {
        return false;
    }

    // layout images in grid and calculate image size from grid
    const auto [image_grid, size] = GridLayout(images, padding);

    // draw new image
    auto composed_image = DrawImage(image_grid, size, background_color);

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
    auto packed = PackImage(image_size, images, padding);

    if(packed.empty())
    {
        return false;
    }

    // optionally reduce image size
    const auto size = pack_image
        ? PackTight(requested_size, &packed, padding)
        : requested_size
        ;

    // border-theory:
    // reuqested_size is decreased with padding
    // the tighlty packed size is increased with padding
    // the packed image-sizes is increased with padding
    // and finally all the images are offseted by padding
    // all to keep padding-sized border between the images
    for(auto& img: packed)
    {
        img.position += vec2i(padding, padding);
    }

    // draw new image
    auto composed_image = DrawImage(packed, size, background_color);

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
            std::vector<std::string> files;

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

            return sub->OnComplete([&]
            {
                const auto was_packed = HandleGrid
                (
                    output_file,
                    padding,
                    background_color,
                    files
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
