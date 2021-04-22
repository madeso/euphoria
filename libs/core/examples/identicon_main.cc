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

    explicit CommonArguments(argparse::parser_base* base)
    {
        base->add("--size", &image_size).set_help("image size");
        base->add("--count", &number_of_images).set_help("The number of images to generate");
        base->set_false("--const", &use_random).set_help("Use a constant value");
        base->add("-t", &type).set_help("Set the type to use");
    }
};


const auto PALETTE = palette
{
    "",
    color::red,
    color::white,
    color::blue,

    color::light_green,
    color::yellow,
    color::light_blue,

    color::pink,
    color::orange,
    color::tan,

    color::brown,
    color::green,
    color::purple,
    color::cornflower_blue,
    color::cyan
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

    auto images = std::vector<image>{};
    image image;
    image.setup_with_alpha_support(image_size, image_size);

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
            RenderSprator(&image, code, PALETTE.get_safe_index(i));
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

            io::ChunkToFile(image.write(image_write_format::png), file_name);
        }
    }

    if(collage)
    {
        std::cout << "writing collage...\n";
        int padding = 20;
        auto collage_image = grid_layout(images, padding, color::gray, true);
        std::string file_name = "identicon.png";
        io::ChunkToFile(collage_image.write(image_write_format::png), file_name);
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
    auto sprites = std::vector<std::vector<image>>{};

    for(int image_index = 0; image_index < number_of_images; image_index += 1)
    {
        auto image_frames = std::vector<image>{};
        for(int frame_index = 0; frame_index < frames; frame_index +=1)
        {
            image image;
            image.setup_with_alpha_support(image_size, image_size);
            image_frames.emplace_back(image);
        }

        const int code = random.NextInteger();
        RenderSprator(&image_frames, code, PALETTE.get_safe_index(image_index));

        sprites.emplace_back(image_frames);
        std::cout << "Generated collage image\n";
    }

    const auto c = Max(frames-2, 0);
    const auto write_frames = c + c*2 + 1;
    for(int anim_index = 0; anim_index < write_frames; anim_index +=1)
    {
        const auto frame_index = (anim_index % 2 == 0) ? 0 : (anim_index+1) / 2;

        auto images = std::vector<image>{};
        for(int image_index = 0; image_index < number_of_images; image_index += 1)
        {
            images.emplace_back(sprites[image_index][frame_index]);
        }

        std::cout << "writing collage...\n";
        int padding = 20;
        auto collage_image = grid_layout(images, padding, color::gray, true);
        std::string file_name = "identicon.png";
        if(frames > 1)
        {
            file_name = Str() << "identicon_" << anim_index << ".png";
        }
        io::ChunkToFile(collage_image.write(image_write_format::png), file_name);
    }
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::parser {"identicon test"};
    auto subs = parser.add_sub_parsers();

    subs->add
    (
        "singles", "write many images",
        [](argparse::sub_parser* sub)
        {
            auto arguments = CommonArguments{sub};
            return sub->on_complete
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
                    return argparse::ok;
                }
            );
        }
    );
    subs->add
    (
        "collage", "write collage",
        [](argparse::sub_parser* sub)
        {
            auto arguments = CommonArguments{sub};
            return sub->on_complete
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
                    return argparse::ok;
                }
            );
        }
    );
    subs->add
    (
        "sprator", "write sprator collage",
        [](argparse::sub_parser* sub)
        {
            int image_size = 100;
            int number_of_images = 9;
            int frames = 3;

            sub->add("--size", &image_size).set_help("Image size");
            sub->add("--images", &number_of_images).set_help("the number of sprators");
            sub->add("--frames", &frames).set_help("the number of anim frames");

            return sub->on_complete
            (
                [&]
                {
                    RunSpratorCollage
                    (
                        image_size,
                        number_of_images,
                        frames
                    );
                    return argparse::ok;
                }
            );
        }
    );

    return argparse::parse_from_main(&parser, argc, argv);
}

