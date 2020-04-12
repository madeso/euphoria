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
#include "core/subvec.h"


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
FindGreatestSortRange(SubVec<Rgbi> colors)
{
    using Tu = std::tuple<SortRange, Range<float>>;

    const auto [min_values, max_values] = FindMinMaxRanges<3, float>
    (
        colors,
        [](const Rgbi& c) -> std::array<float, 3>
        {
            return
            {
                GetValue(SortRange::R, c),
                GetValue(SortRange::G, c),
                GetValue(SortRange::B, c)
            };
        }
    );
    // init-capture due to a bug in clang/gcc/standard
    // https://www.reddit.com/r/cpp/comments/68vhir/whats_the_rationale_for_this_reference_to_local/
    auto make = [&, miv = std::move(min_values), mav = std::move(max_values)]
        (SortRange r, size_t i) -> Tu
        {
            return std::make_tuple(r, Range{miv[i], mav[i]});
        }
        ;
    auto ranges = std::vector<Tu>
    {
        make(SortRange::R, 0),
        make(SortRange::G, 1),
        make(SortRange::B, 2)
    };
    std::sort(ranges.begin(), ranges.end(), [](const Tu& lhs, const Tu& rhs)
    {
        return std::get<1>(lhs).GetRange() > std::get<1>(rhs).GetRange();
    });
    return ranges[0];
}


void
Sort(SortRange sort_range, SubVec<Rgbi> colors)
{
    auto sort = [&](std::function<float (const Rgbi& c)> conv)
    {
        std::sort(colors.begin(), colors.end(), [&]
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
FindMedianIndex(SortRange sort, SubVec<Rgbi> colors, Range<float> range)
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
MedianCut(SubVec<Rgbi> src, int depth)
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

    Sort(range, src);

    // const auto median_index = src.size() / 2;
    const auto median = FindMedianIndex(range, src, rrange);
    const auto left = src.Sub(0, median);
    const auto right = src.Sub(median, src.size());

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
        ret.reserve(ret.size() + img.image.GetHeight() * img.image.GetWidth());
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
    auto all_colors = ExtractAllColors(images);
    auto colors = MedianCut(SubVec{&all_colors}, depth);

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
    auto all_colors = ExtractAllColors(images);
    auto colors = MedianCut(SubVec{&all_colors}, depth);

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
