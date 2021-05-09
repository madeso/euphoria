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
    extract_all_colors(const image& image)
    {
        auto ret = std::vector<rgbi>{};
        ret.reserve(ret.size() + image.height * image.width);

        for(int y=0; y<image.height; y+=1)
        {
            for(int x=0; x<image.width; x+=1)
            {
                const auto color = crgbi(image.get_pixel(x, y));
                ret.emplace_back(color);
            }
        }

        return ret;
    }


    enum class sort_range
    {
        R, G, B
    };


    float
    get_value(sort_range range, const rgbi& c)
    {
        switch(range)
        {
            case sort_range::R: return crgb(c).r;
            case sort_range::G: return crgb(c).g;
            case sort_range::B: return crgb(c).b;
            default: return 0;
        }
    }


    std::tuple<sort_range, range<float>>
    find_greatest_sort_range(subvec<rgbi> colors)
    {
        using Tu = std::tuple<sort_range, range<float>>;

        const auto [min_values, max_values] = find_min_max_ranges<3, float>
        (
            colors,
            [](const rgbi& c) -> std::array<float, 3>
            {
                return
                {
                    get_value(sort_range::R, c),
                    get_value(sort_range::G, c),
                    get_value(sort_range::B, c)
                };
            }
        );
        // init-capture due to a bug in clang/gcc/standard
        // https://www.reddit.com/r/cpp/comments/68vhir/whats_the_rationale_for_this_reference_to_local/
        auto make = [&, miv = std::move(min_values), mav = std::move(max_values)] (sort_range r, size_t i) -> Tu
        {
            return std::make_tuple(r, range{miv[i], mav[i]});
        };
        auto ranges = std::vector<Tu>
        {
            make(sort_range::R, 0),
            make(sort_range::G, 1),
            make(sort_range::B, 2)
        };
        std::sort
        (
            ranges.begin(), ranges.end(), [](const Tu& lhs, const Tu& rhs)
            {
                return std::get<1>(lhs).get_distance() > std::get<1>(rhs).get_distance();
            }
        );
        return ranges[0];
    }


    void
    sort(sort_range sort_range, subvec<rgbi> colors)
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
        case sort_range::R: sort([](const rgbi& c) -> float { return c.r; }); break;
        case sort_range::G: sort([](const rgbi& c) -> float { return c.g; }); break;
        case sort_range::B: sort([](const rgbi& c) -> float { return c.b; }); break;
        }
    }



    size_t
    find_median_index(sort_range sort, subvec<rgbi> colors, range<float> range)
    {
        const auto median = range.get_distance()/2 + range.lower_bound;
        // todo(Gustav): make non-linear
        for(size_t index = 0; index<colors.size()-1; index+=1)
        {
            if(get_value(sort, colors[Csizet_to_int(index+1)]) >= median)
            {
                return index;
            }
        }
        DIE("shouldn't happen...");
        return 0;
    }




    std::vector<rgbi>
    median_cut(subvec<rgbi> src, int depth, bool split_middle)
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

        const auto [range, rrange] = find_greatest_sort_range(src);

        sort(range, src);

        const auto median = split_middle
            ? src.size() / 2
            : find_median_index(range, src, rrange)
            ;
        const auto left = src.sub(0, median);
        const auto right = src.sub(median, src.size());

        auto ret = median_cut(left, depth - 1, split_middle);
        const auto rhs = median_cut(right, depth - 1, split_middle);
        ret.insert(ret.end(), rhs.begin(), rhs.end());

        return ret;
    }
}

namespace euphoria::core
{
    std::vector<rgbi>
    median_cut(const image& image, int depth, bool middle_split)
    {
        auto all_colors = extract_all_colors(image);
        auto colors = ::median_cut(subvec{&all_colors}, depth, middle_split);
        return colors;
    }
}

