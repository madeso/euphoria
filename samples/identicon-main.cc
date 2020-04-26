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

using namespace euphoria::core;


enum class HashType
{
    Identicon, Retro, Sprator
};


int
main(int argc, char* argv[])
{
    auto image_size = 512;
    auto number_of_images = 10;
    bool use_random = true;
    bool collage = false;
    HashType type = HashType::Identicon;

    auto parser = argparse::Parser {"identicon test"};
    parser.Add("--size", &image_size).Help("image size");
    parser.Add("--count", &number_of_images).Help("The number of images to generate");
    parser.SetFalse("--const", &use_random).Help("Use a constant value");
    parser.SetTrue("--collage", &collage).Help("output to a single collage istead of several images");
    parser.Add("-t", &type).Help("Set the type to use");

    if(const auto r = parser.Parse(argc, argv))
    {
        return *r;
    }

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
            RenderSprator(&image, code);
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

    return 0;
}

