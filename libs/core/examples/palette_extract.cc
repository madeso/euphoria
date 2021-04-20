#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <utility>
#include <optional>
#include <map>

#include "core/argparse.h"
#include "core/io.h"
#include "core/image.h"
#include "core/cint.h"
#include "core/image_draw.h"
#include "core/timepoint.h"
#include "core/palette_extract.h"


using namespace euphoria::core;


struct ImageAndFile
{
    ImageAndFile() = default;

    ImageAndFile(const std::string& f, const Image& i)
        : file(f)
        , image(i)
    {}

    std::string file;
    Image image;
};


std::vector<ImageAndFile>
LoadImages(const std::vector<std::string>& files)
{
    auto images = std::vector<ImageAndFile>{};

    for(const auto& f: files)
    {
        auto chunk = io::FileToChunk(f);
        if(chunk == nullptr)
        {
            std::cerr << "failed to read image file " << f << "\n";
            return {};
        }
        auto loaded_image = LoadImage(chunk, f, AlphaLoad::Keep);
        if(loaded_image.error.empty() == false)
        {
            std::cerr << "failed to read image data " <<
                loaded_image.error << "\n";
            return {};
        }

        images.emplace_back(f, loaded_image.image);
    }

    return images;
}


struct ExtractedColor
{
    ExtractedColor(const Rgbi& r, int c)
        : color(r)
        , count(c)
    {
    }

    Rgbi color;
    int count;
};


int
Find(std::vector<ExtractedColor>* psource, const Rgbi& color, float length)
{
    auto& source = *psource;

    for(int i=0; i<Csizet_to_int(source.size()); i+=1)
    {
        if((rgb(source[i].color) - rgb(color)).GetLength()*255 < length)
        {
            return i;
        }
    }

    source.emplace_back(color, 0);
    return Csizet_to_int(source.size()) - 1;
}

std::vector<ExtractedColor>
ExtractColors(const std::vector<ImageAndFile>& images, float range)
{
    auto ret = std::vector<ExtractedColor>{};

    for(const auto& img: images)
    {
        for(int y=0; y<img.image.GetHeight(); y+=1)
        {
            for(int x=0; x<img.image.GetWidth(); x+=1)
            {
                const auto index = Find(&ret, rgbi(img.image.GetPixel(x,y)), range);
                ret[index].count += 1;
            }
        }
    }

    return ret;
}


std::vector<ExtractedColor>
ExtractColors(const std::vector<ImageAndFile>& images)
{
    std::map<int, int> colors;
    for(const auto& img: images)
    {
        for(int y=0; y<img.image.GetHeight(); y+=1)
        {
            for(int x=0; x<img.image.GetWidth(); x+=1)
            {
                const auto color = rgbi(img.image.GetPixel(x, y));
                const auto hex = color.ToHex();
                auto val = std::get<0>(colors.try_emplace(hex, 0));
                val->second += 1;
            }
        }
    }

    auto ret = std::vector<ExtractedColor>{};
    for(const auto c: colors)
    {
        ret.emplace_back(Rgbi::FromHex(c.first), c.second);
    }
    return ret;
}


bool
HandleImage
(
    const std::vector<std::string>& files,
    int depth,
    bool middle_split,
    int image_size,
    const std::string& file
)
{
    // load images
    const auto images = LoadImages(files);
    if(images.size() != 1)
    {
        std::cerr << "currently only one image supported...\n";
        return false;
    }

    const auto start = Now();

    // extract colors
    auto colors = MedianCut(images[0].image, depth, middle_split);

    const auto end = Now();

    const auto seconds = SecondsBetween(start, end);
    std::cout << "Palette extraction took " << seconds << " seconds.\n";

    if(colors.empty())
    {
        return false;
    }

    Image image;
    image.SetupNoAlphaSupport(image_size * Csizet_to_int(colors.size()), image_size);
    for
    (
        int i = 0;
        i < Csizet_to_int(colors.size());
        i += 1
    )
    {
        DrawSquare
        (
            &image,
            colors[i],
            image_size * i,
            image_size - 1,
            image_size
        );
    }

    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), file);
    return true;
}


bool
HandlePrint
(
    const std::vector<std::string>& files,
    int depth,
    bool middle_split
)
{
    // load images
    const auto images = LoadImages(files);
    if(images.size() != 1)
    {
        std::cerr << "currently only one image supported...\n";
        return false;
    }

    // extract colors
    auto colors = MedianCut(images[0].image, depth, middle_split);

    std::sort
    (
        colors.begin(),
        colors.end(),
        [](const Rgbi& lhs, const Rgbi& rhs) -> bool
        {
            return rgb(lhs).CalcLuminance() < rgb(rhs).CalcLuminance();
        }
    );

    if(colors.empty())
    {
        return false;
    }

    // print colors
    for(auto c: colors)
    {
        std::cout << c << "\n";
    }

    std::cout << "Found #colors: " << colors.size() << "\n";
    return true;
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::parser
    {
        "extract colors from images"
    };
    auto subs = parser.add_sub_parsers();

    subs->add
    (
        "print", "print all colors found in images",
        [](argparse::sub_parser* sub)
        {
            std::vector<std::string> files;
            int depth = 3;
            bool middle_split = false;

            sub->add("--depth", &depth)
                .set_allow_before_positionals()
                .set_nargs("R")
                .set_help("change the palette depth")
                ;
            sub->set_true("--middle", &middle_split)
                .set_allow_before_positionals()
                .set_help("split at middle array insteaf of median")
                ;
            sub->add_vector("files", &files)
                .set_nargs("F")
                .set_help("the files to analyze")
                ;

            return sub->on_complete([&]
            {
                const auto was_extracted = HandlePrint
                (
                    files, depth, middle_split
                );

                return was_extracted
                    ? argparse::ok
                    : argparse::error
                    ;
            });
        }
    );


    subs->add
    (
        "image", "write all colors found in images to a palette image",
        [](argparse::sub_parser* sub)
        {
            int image_size = 5;
            std::string file = "pal.png";
            std::vector<std::string> files;
            int depth = 3;
            bool middle_split = false;

            sub->add("--depth", &depth)
                .set_allow_before_positionals()
                .set_nargs("R")
                .set_help("change the palette depth")
                ;
            sub->set_true("--middle", &middle_split)
                .set_allow_before_positionals()
                .set_help("split at middle array insteaf of median")
                ;
            sub->add("--size", &image_size)
                .set_allow_before_positionals()
                .set_nargs("S")
                .set_help("change the image size")
                ;
            sub->add("-o, --output", &file)
                .set_allow_before_positionals()
                .set_nargs("FILE")
                .set_help("change where to save the output")
                ;
            sub->add_vector("files", &files)
                .set_nargs("F")
                .set_help("the files to analyze")
                ;

            return sub->on_complete([&]
            {
                const auto was_extracted = HandleImage
                (
                    files,
                    depth,
                    middle_split,
                    image_size,
                    file
                );

                return was_extracted
                    ? argparse::ok
                    : argparse::error
                    ;
            });
        }
    );


    return parse_from_main(&parser, argc, argv);
}
