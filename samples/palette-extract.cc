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
#include "core/minmax.h"
#include "core/timepoint.h"


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

enum class SortRange
{
    R, G, B
};


float
GetValue(SortRange range, const Rgbi& c)
{
    switch(range)
    {
        case SortRange::R: return rgb(c).r;
        case SortRange::G: return rgb(c).g;
        case SortRange::B: return rgb(c).b;
        default: return 0;
    }
}


std::tuple<SortRange, Range<float>>
FindGreatestSortRange(const std::vector<Rgbi>& colors)
{
    using Tu = std::tuple<SortRange, Range<float>>;

    // todo(Gustav): this iterates 3 times through the color array, make it 1
    // todo(Gustav): remove the std::function and use GetValue(SortRange)
    auto make = [&]
    (SortRange r, std::function<float (const Rgbi& c)> conv) -> Tu
    {
        return std::make_tuple(r, FindMinMaxRange<float>(colors, conv));
    };
    auto ranges = std::vector<Tu>
    {
        make(SortRange::R, [](const Rgbi& c) -> float { return rgb(c).r; }),
        make(SortRange::G, [](const Rgbi& c) -> float { return rgb(c).g; }),
        make(SortRange::B, [](const Rgbi& c) -> float { return rgb(c).b; })
    };
    std::sort(ranges.begin(), ranges.end(), [](const Tu& lhs, const Tu& rhs)
    {
        return std::get<1>(lhs).GetRange() > std::get<1>(rhs).GetRange();
    });
    return ranges[0];
}


void
Sort(SortRange sort_range, std::vector<Rgbi>* colors)
{
    auto sort = [&](std::function<float (const Rgbi& c)> conv)
    {
        std::sort(colors->begin(), colors->end(), [&]
        (
            const Rgbi& lhs, const Rgbi& rhs)
            {
                return conv(lhs) < conv(rhs);
            }
        );
    };

    switch(sort_range)
    {
    case SortRange::R: sort([](const Rgbi& c) -> float { return c.r; }); break;
    case SortRange::G: sort([](const Rgbi& c) -> float { return c.g; }); break;
    case SortRange::B: sort([](const Rgbi& c) -> float { return c.b; }); break;
    }
}


size_t
FindMedianIndex(SortRange sort, const std::vector<Rgbi>& colors, Range<float> range)
{
    const auto median = range.GetRange()/2 + range.lower_bound;
    // todo(Gustav): make non-linear
    for(size_t index = 0; index<colors.size()-1; index+=1)
    {
        if(GetValue(sort, colors[index+1]) >= median)
        {
            return index;
        }
    }
    DIE("shouldn't happen...");
    return 0;
}


std::vector<Rgbi>
MedianCut(const std::vector<Rgbi>& src, int depth)
{
    if(src.empty())
    {
        return {};
    }

    if(depth <= 0 || src.size() < 2)
    {
        auto sum = Rgb{0,0,0};
        float total = 0;
        for(const auto& c: src)
        {
            sum += rgb(c);
            total += 1;
        }

        const auto r = rgbi(sum / total);

        return {r};
    }

    const auto [range, rrange] = FindGreatestSortRange(src);

    using V = std::vector<Rgbi>;
    V vec = src;
    Sort(range, &vec);

    // const auto median_index = vec.size() / 2;
    const auto median_index = FindMedianIndex(range, vec, rrange);
    const auto median = vec.begin()+median_index;
    const auto left = V(vec.begin(), median);
    const auto right = V(median, vec.end());

    auto ret = MedianCut(left, depth -1);
    const auto rhs = MedianCut(right, depth-1);
    ret.insert(ret.end(), rhs.begin(), rhs.end());

    return ret;
}


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



std::vector<Rgbi>
ExtractAllColors(const std::vector<ImageAndFile>& images)
{
    auto ret = std::vector<Rgbi>{};
    for(const auto& img: images)
    {
        for(int y=0; y<img.image.GetHeight(); y+=1)
        for(int x=0; x<img.image.GetWidth(); x+=1)
        {
            const auto color = rgbi(img.image.GetPixel(x, y));
            ret.emplace_back(color);
        }
    }

    return ret;
}


bool
HandleImage
(
    const std::vector<std::string>& files,
    int depth,
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

    const auto start = Now();

    // extract colors
    auto colors = MedianCut(ExtractAllColors(images), depth);

    const auto end = Now();

    const auto seconds = SecondsBetween(start, end);
    std::cout << "Palette extraction took " << seconds << " seconds.\n";

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
    int depth
)
{
    // load images
    const auto images = LoadImages(files);
    if(images.empty())
    {
        return false;
    }

    // extract colors
    auto colors = MedianCut(ExtractAllColors(images), depth);

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
            int depth = 3;

            sub->Add("--depth", &depth)
                .AllowBeforePositionals()
                .Nargs("R")
                .Help("change the palette depth")
                ;
            sub->AddVector("files", &files)
                .Nargs("F")
                .Help("the files to analyze")
                ;

            return sub->OnComplete([&]
            {
                const auto was_extracted = HandlePrint
                (
                    files, depth
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
            int depth = 3;

            sub->Add("--depth", &depth)
                .AllowBeforePositionals()
                .Nargs("R")
                .Help("change the palette depth")
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
                    depth,
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
