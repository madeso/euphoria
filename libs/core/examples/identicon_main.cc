#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/hashgen.h"
#include "core/argparse.h"
#include "core/random.h"
#include "core/collage.h"
#include "core/palette.h"

using namespace euphoria;
using namespace euphoria::core;


enum class HashType
{
    identicon, retro, sprator
};


struct CommonArguments
{
    int image_size = 512;
    int number_of_images = 10;
    bool use_random = true;
    HashType type = HashType::identicon;

    explicit CommonArguments(argparse::ParserBase* base)
    {
        base->add("--size", &image_size).set_help("image size");
        base->add("--count", &number_of_images).set_help("The number of images to generate");
        base->set_false("--const", &use_random).set_help("Use a constant value");
        base->add("-t", &type).set_help("Set the type to use");
    }
};


void run_main
(
    int image_size,
    int number_of_images,
    bool use_random,
    HashType type,
    bool collage,
    const euphoria::core::Palette& palette
)
{
    if(use_random==false && number_of_images > 1)
    {
        std::cout << "Since random isn't requested: setting the number of images to 1...\n";
        number_of_images = 1;
    }

    core::Random random;

    auto images = std::vector<Image>{};
    Image image;
    image.setup_with_alpha_support(image_size, image_size);

    for(int i = 0; i < number_of_images; i += 1)
    {
        U32 code = 42;
        if(use_random)
        {
            code = random.get_next_u32();
        }

        switch(type)
        {
        case HashType::identicon:
            render_identicon(&image, code);
            break;
        case HashType::retro:
            render_retro(&image, code);
            break;
        case HashType::sprator:
            render_sprator(&image, code, palette.get_safe_index(i));
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
                file_name = "identicon_{}.png"_format(i + 1);
                std::cout << "Writing " << file_name << "\n";
            }

            io::write_chunk_to_file(image.write(ImageWriteFormat::png), file_name);
        }
    }

    if(collage)
    {
        std::cout << "writing collage...\n";
        int padding = 20;
        auto collage_image = lay_out_in_a_grid(images, padding, NamedColor::gray, true);
        std::string file_name = "identicon.png";
        io::write_chunk_to_file(collage_image.write(ImageWriteFormat::png), file_name);
    }
}


void run_sprator_collage
(
    int image_size,
    int number_of_images,
    int frames,
    const euphoria::core::Palette& palette
)
{
    core::Random random;
    auto sprites = std::vector<std::vector<Image>>{};

    for(int image_index = 0; image_index < number_of_images; image_index += 1)
    {
        auto image_frames = std::vector<Image>{};
        for(int frame_index = 0; frame_index < frames; frame_index +=1)
        {
            Image image;
            image.setup_with_alpha_support(image_size, image_size);
            image_frames.emplace_back(image);
        }

        const auto code = random.get_next_u32();
        render_sprator(&image_frames, code, palette.get_safe_index(image_index));

        sprites.emplace_back(image_frames);
        std::cout << "Generated collage image\n";
    }

    const auto c = max(frames-2, 0);
    const auto write_frames = c + c*2 + 1;
    for(int anim_index = 0; anim_index < write_frames; anim_index +=1)
    {
        const auto frame_index = (anim_index % 2 == 0) ? 0 : (anim_index+1) / 2;

        auto images = std::vector<Image>{};
        for(int image_index = 0; image_index < number_of_images; image_index += 1)
        {
            images.emplace_back(sprites[image_index][frame_index]);
        }

        std::cout << "writing collage...\n";
        int padding = 20;
        auto collage_image = lay_out_in_a_grid(images, padding, NamedColor::gray, true);
        std::string file_name = "identicon.png";
        if(frames > 1)
        {
            file_name = "identicon_{}.png"_format(anim_index);
        }
        io::write_chunk_to_file(collage_image.write(ImageWriteFormat::png), file_name);
    }
}


int
main(int argc, char* argv[])
{
    const auto palette = euphoria::core::Palette
    {
        "",
        std::array
        {
            rgbi{NamedColor::red},
            rgbi{NamedColor::white},
            rgbi{NamedColor::blue},

            rgbi{NamedColor::light_green},
            rgbi{NamedColor::yellow},
            rgbi{NamedColor::light_blue},

            rgbi{NamedColor::pink},
            rgbi{NamedColor::orange},
            rgbi{NamedColor::tan},

            rgbi{NamedColor::brown},
            rgbi{NamedColor::green},
            rgbi{NamedColor::purple},
            rgbi{NamedColor::cornflower_blue},
            rgbi{NamedColor::cyan}
        }
    };

    auto parser = argparse::Parser {"identicon test"};
    auto subs = parser.add_sub_parsers();

    subs->add
    (
        "singles", "write many images",
        [&palette](argparse::SubParser* sub)
        {
            auto arguments = CommonArguments{sub};
            return sub->on_complete
            (
                [&]
                {
                    run_main
                    (
                        arguments.image_size,
                        arguments.number_of_images,
                        arguments.use_random,
                        arguments.type,
                        false,
                        palette
                    );
                    return argparse::ok;
                }
            );
        }
    );
    subs->add
    (
        "collage", "write collage",
        [&palette](argparse::SubParser* sub)
        {
            auto arguments = CommonArguments{sub};
            return sub->on_complete
            (
                [&]
                {
                    run_main
                    (
                        arguments.image_size,
                        arguments.number_of_images,
                        arguments.use_random,
                        arguments.type,
                        true,
                        palette
                    );
                    return argparse::ok;
                }
            );
        }
    );
    subs->add
    (
        "sprator", "write sprator collage",
        [&palette](argparse::SubParser* sub)
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
                    run_sprator_collage
                    (
                        image_size,
                        number_of_images,
                        frames,
                        palette
                    );
                    return argparse::ok;
                }
            );
        }
    );

    return argparse::parse_from_main(&parser, argc, argv);
}

