#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/identicon.h"
#include "core/hashgen_retro.h"
#include "core/argparse.h"
#include "core/random.h"
#include "core/str.h"

using namespace euphoria::core;


int
main(int argc, char* argv[])
{
    auto image_size = 512;
    auto number_of_images = 10;
    bool use_random = true;
    bool use_identicon = true;

    auto parser = argparse::Parser {"identicon test"};
    parser.Add("-size", &image_size).Help("image size");
    parser.Add("-count", &number_of_images).Help("The number of images to generate");
    parser.SetFalse("-const", &use_random).Help("Use a constant value");
    parser.SetFalse("-retro", &use_identicon).Help("Use retro render instead");

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

    Image image;
    image.SetupNoAlphaSupport(image_size, image_size);

    for(int i = 0; i < number_of_images; i += 1)
    {
        int code = -2044886870;
        if(use_random)
        {
            code = random.NextInteger();
        }

        if(use_identicon)
        {
            RenderIdenticon(&image, code);
        }
        else
        {
            RenderRetro(&image, code);
        }

        std::string file_name = "identicon.png";

        if(number_of_images > 1)
        {
            file_name = Str() << "identicon_" << (i + 1) << ".png";
            std::cout << "Writing " << file_name << "\n";
        }

        io::ChunkToFile(image.Write(ImageWriteFormat::PNG), file_name);
    }

    return 0;
}

