#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/identicon.h"
#include "core/hashgen_retro.h"
#include "core/hashgen_sprator.h"
#include "core/argparse.h"
#include "core/random.h"
#include "core/str.h"
#include "core/collage.h"
#include "core/palette.h"

using namespace euphoria::core;


enum class HashType
{
    Identicon, Retro, Sprator
};


struct CommonArguments
{
    int image_size = 512;
    int number_of_images = 10;
    bool use_random = true;
    HashType type = HashType::Identicon;

    explicit CommonArguments(argparse::ParserBase* base)
    {
        base->Add("--size", &image_size).Help("image size");
        base->Add("--count", &number_of_images).Help("The number of images to generate");
        base->SetFalse("--const", &use_random).Help("Use a constant value");
        base->Add("-t", &type).Help("Set the type to use");
    }
};


const auto PALETTE = Palette
{
    "",
    Color::Red,
    Color::White,
    Color::Blue,

    Color::LightGreen,
    Color::Yellow,
    Color::LightBlue,

    Color::Pink,
    Color::Orange,
    Color::Tan,

    Color::Brown,
    Color::Green,
    Color::Purple,
    Color::CornflowerBlue,
    Color::Cyan
};



void RunMain
(
    int image_size,
    int number_of_images,
    bool use_random,
    HashType type,
    bool collage
)
{
    if(use_random==false && number_of_images > 1)
    {
        std::cout << "Since random isn't requested: setting the number of images to 1...\n";
        number_of_images = 1;
    }

    Random random;

    auto images = std::vector<Image>{};
    Image image;
    image.SetupWithAlphaSupport(image_size, image_size);

    for(int i = 0; i < number_of_images; i += 1)
    {
        int code = -2044886870;
        if(use_random)
        {
            code = random.NextInteger();
        }

        switch(type)
        {
        case HashType::Identicon:
            RenderIdenticon(&image, code);
            break;
        case HashType::Retro:
            RenderRetro(&image, code);
            break;
        case HashType::Sprator:
            RenderSprator(&image, code, PALETTE.GetSafeIndex(i));
            break;
        default:
            DIE("Unhandled type");
            break;
        }

        std::string file_name = "identicon.png";

        if(collage)
        {
            images.emplace_back(image);
            std::cout << "Generated collage image\n";
        }
        else
        {
            if(number_of_images > 1)
            {
                file_name = Str() << "identicon_" << (i + 1) << ".png";
                std::cout << "Writing " << file_name << "\n";
            }

            io::ChunkToFile(image.Write(ImageWriteFormat::PNG), file_name);
        }
    }

    if(collage)
    {
        std::cout << "writing collage...\n";
        int padding = 20;
        auto collage_image = GridLayout(images, padding, Color::Gray, true);
        std::string file_name = "identicon.png";
        io::ChunkToFile(collage_image.Write(ImageWriteFormat::PNG), file_name);
    }
}


void RunSpratorCollage
(
    int image_size,
    int number_of_images,
    int frames
)
{
    Random random;
    auto sprites = std::vector<std::vector<Image>>{};

    for(int image_index = 0; image_index < number_of_images; image_index += 1)
    {
        auto image_frames = std::vector<Image>{};
        for(int frame_index = 0; frame_index < frames; frame_index +=1)
        {
            Image image;
            image.SetupWithAlphaSupport(image_size, image_size);
            image_frames.emplace_back(image);
        }

        const int code = random.NextInteger();
        RenderSprator(&image_frames, code, PALETTE.GetSafeIndex(image_index));

        sprites.emplace_back(image_frames);
        std::cout << "Generated collage image\n";
    }

    const auto c = Max(frames-2, 0);
    const auto write_frames = c + c*2 + 1;
    for(int image_index = 0; image_index < write_frames; image_index +=1)
    {
        const auto frame_index = (image_index % 2 == 0) ? 0 : (image_index+1) / 2;

        auto images = std::vector<Image>{};
        for(int image_index = 0; image_index < number_of_images; image_index += 1)
        {
            images.emplace_back(sprites[image_index][frame_index]);
        }

        std::cout << "writing collage...\n";
        int padding = 20;
        auto collage_image = GridLayout(images, padding, Color::Gray, true);
        std::string file_name = "identicon.png";
        if(frames > 1)
        {
            file_name = Str() << "identicon_" << image_index << ".png";
        }
        io::ChunkToFile(collage_image.Write(ImageWriteFormat::PNG), file_name);
    }
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser {"identicon test"};
    auto subs = parser.AddSubParsers();

    subs->Add
    (
        "singles", "write many images",
        [](argparse::SubParser* sub)
        {
            auto arguments = CommonArguments{sub};
            return sub->OnComplete
            (
                [&]
                {
                    RunMain
                    (
                        arguments.image_size,
                        arguments.number_of_images,
                        arguments.use_random,
                        arguments.type,
                        false
                    );
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );
    subs->Add
    (
        "collage", "write collage",
        [](argparse::SubParser* sub)
        {
            auto arguments = CommonArguments{sub};
            return sub->OnComplete
            (
                [&]
                {
                    RunMain
                    (
                        arguments.image_size,
                        arguments.number_of_images,
                        arguments.use_random,
                        arguments.type,
                        true
                    );
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );
    subs->Add
    (
        "sprator", "write sprator collage",
        [](argparse::SubParser* sub)
        {
            int image_size = 100;
            int number_of_images = 9;
            int frames = 3;

            sub->Add("--size", &image_size).Help("Image size");
            sub->Add("--images", &number_of_images).Help("the number of sprators");
            sub->Add("--frames", &frames).Help("the number of anim frames");

            return sub->OnComplete
            (
                [&]
                {
                    RunSpratorCollage
                    (
                        image_size,
                        number_of_images,
                        frames
                    );
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    return argparse::ParseFromMain(&parser, argc, argv);
}

