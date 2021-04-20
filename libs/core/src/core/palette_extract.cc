#include "core/palette_extract.h"

#include <functional>
#include <algorithm>

#include "core/minmax.h"
#include "core/subvec.h"
#include "core/image.h"
#include "core/cint.h"


namespace
{
    using namespace euphoria::core;


    std::vector<rgbi>
    ExtractAllColors(const image& image)
    {
        auto ret = std::vector<rgbi>{};
        ret.reserve(ret.size() + image.get_height() * image.get_width());

        for(int y=0; y<image.get_height(); y+=1)
        {
            for(int x=0; x<image.get_width(); x+=1)
            {
                const auto color = crgbi(image.get_pixel(x, y));
                ret.emplace_back(color);
            }
        }

        return ret;
    }


    enum class SortRange
    {
        R, G, B
    };


    float
    GetValue(SortRange range, const rgbi& c)
    {
        switch(range)
        {
            case SortRange::R: return crgb(c).r;
            case SortRange::G: return crgb(c).g;
            case SortRange::B: return crgb(c).b;
            default: return 0;
        }
    }


    std::tuple<SortRange, Range<float>>
    FindGreatestSortRange(SubVec<rgbi> colors)
    {
        using Tu = std::tuple<SortRange, Range<float>>;

        const auto [min_values, max_values] = FindMinMaxRanges<3, float>
        (
            colors,
            [](const rgbi& c) -> std::array<float, 3>
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
    Sort(SortRange sort_range, SubVec<rgbi> colors)
    {
        auto sort = [&](std::function<float (const rgbi& c)> conv)
        {
            std::sort(colors.begin(), colors.end(), [&]
            (
                const rgbi& lhs, const rgbi& rhs)
                {
                    return conv(lhs) < conv(rhs);
                }
            );
        };

        switch(sort_range)
        {
        case SortRange::R: sort([](const rgbi& c) -> float { return c.r; }); break;
        case SortRange::G: sort([](const rgbi& c) -> float { return c.g; }); break;
        case SortRange::B: sort([](const rgbi& c) -> float { return c.b; }); break;
        }
    }



    size_t
    FindMedianIndex(SortRange sort, SubVec<rgbi> colors, Range<float> range)
    {
        const auto median = range.GetRange()/2 + range.lower_bound;
        // todo(Gustav): make non-linear
        for(size_t index = 0; index<colors.size()-1; index+=1)
        {
            if(GetValue(sort, colors[Csizet_to_int(index+1)]) >= median)
            {
                return index;
            }
        }
        DIE("shouldn't happen...");
        return 0;
    }




    std::vector<rgbi>
    MedianCut(SubVec<rgbi> src, int depth, bool split_middle)
    {
        if(src.empty())
        {
            return {};
        }

        if(depth <= 0 || src.size() < 2)
        {
            auto sum = rgb{0,0,0};
            float total = 0;
            for(const auto& c: src)
            {
                sum += crgb(c);
                total += 1;
            }

            const auto r = crgbi(sum / total);

            return {r};
        }

        const auto [range, rrange] = FindGreatestSortRange(src);

        Sort(range, src);

        const auto median = split_middle
            ? src.size() / 2
            : FindMedianIndex(range, src, rrange)
            ;
        const auto left = src.Sub(0, median);
        const auto right = src.Sub(median, src.size());

        auto ret = MedianCut(left, depth - 1, split_middle);
        const auto rhs = MedianCut(right, depth - 1, split_middle);
        ret.insert(ret.end(), rhs.begin(), rhs.end());

        return ret;
    }
}

namespace euphoria::core
{
    std::vector<rgbi>
    MedianCut(const image& image, int depth, bool middle_split)
    {
        auto all_colors = ExtractAllColors(image);
        auto colors = ::MedianCut(SubVec{&all_colors}, depth, middle_split);
        return colors;
    }
}

