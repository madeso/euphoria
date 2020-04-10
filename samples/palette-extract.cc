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

    for(int i=0; i<source.size(); i+=1)
    {
        if((rgb(source[i].color) - rgb(color)).GetLength()*255 < length)
        {
            return i;
        }
    }

    source.emplace_back(color, 0);
    return source.size() - 1;
}

std::vector<ExtractedColor>
ExtractColors(const std::vector<ImageAndFile>& images, float range)
{
    auto ret = std::vector<ExtractedColor>{};

    for(const auto& img: images)
    {
        for(int y=0; y<img.image.GetHeight(); y+=1)
        for(int x=0; x<img.image.GetWidth(); x+=1)
        {
            const auto index = Find(&ret, rgbi(img.image.GetPixel(x,y)), range);
            ret[index].count += 1;
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
        for(int x=0; x<img.image.GetWidth(); x+=1)
        {
            const auto color = rgbi(img.image.GetPixel(x, y));
            const auto hex = color.ToHex();
            auto val = std::get<0>(colors.try_emplace(hex, 0));
            val->second += 1;
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
    float range,
    int image_size,
    const std::string& file
)
{
    // load images
    const auto images = LoadImages(files);
    if(images.empty())
    {
        return false;
    }

    // extract colors
    auto colors = ExtractColors(images, range);

    if(colors.empty())
    {
        return false;
    }

    Image image;
    image.SetupNoAlphaSupport(image_size * colors.size(), image_size);
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
            colors[i].color,
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
    float range
)
{
    // load images
    const auto images = LoadImages(files);
    if(images.empty())
    {
        return false;
    }

    // extract colors
    auto colors = ExtractColors(images, range);

    if(colors.empty())
    {
        return false;
    }

    // print colors
    for(auto c: colors)
    {
        std::cout << c.color << " with " << +c.count << "\n";
    }

    std::cout << "Found #colors: " << colors.size() << "\n";
    return true;
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser
    {
        "extract colors from images"
    };
    auto subs = parser.AddSubParsers();

    subs->Add
    (
        "print", "print all colors found in images",
        [](argparse::SubParser* sub)
        {
            std::vector<std::string> files;
            float range = 5;

            sub->Add("--range", &range)
                .AllowBeforePositionals()
                .Nargs("R")
                .Help("change the pixel range")
                ;
            sub->AddVector("files", &files)
                .Nargs("F")
                .Help("the files to analyze")
                ;

            return sub->OnComplete([&]
            {
                const auto was_extracted = HandlePrint
                (
                    files, range
                );

                return was_extracted
                    ? argparse::ParseResult::Ok
                    : argparse::ParseResult::Error
                    ;
            });
        }
    );


    subs->Add
    (
        "image", "write all colors found in images to a palette image",
        [](argparse::SubParser* sub)
        {
            int image_size = 5;
            std::string file = "pal.png";
            std::vector<std::string> files;
            float range = 5;

            sub->Add("--range", &range)
                .AllowBeforePositionals()
                .Nargs("R")
                .Help("change the pixel range")
                ;
            sub->Add("--size", &image_size)
                .AllowBeforePositionals()
                .Nargs("S")
                .Help("change the image size")
                ;
            sub->Add("-o, --output", &file)
                .AllowBeforePositionals()
                .Nargs("FILE")
                .Help("change where to save the output")
                ;
            sub->AddVector("files", &files)
                .Nargs("F")
                .Help("the files to analyze")
                ;

            return sub->OnComplete([&]
            {
                const auto was_extracted = HandleImage
                (
                    files,
                    range,
                    image_size,
                    file
                );

                return was_extracted
                    ? argparse::ParseResult::Ok
                    : argparse::ParseResult::Error
                    ;
            });
        }
    );


    return ParseFromMain(&parser, argc, argv);
}
