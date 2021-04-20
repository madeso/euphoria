#include "core/palette.h"

#include "core/assert.h"
#include "core/numeric.h"
#include "core/random.h"
#include "core/colors.h"


namespace euphoria::core
{
    namespace
    {
        float
        Diff(const rgbi& lhs, const rgbi& rhs)
        {
            return
                Square(static_cast<float>(lhs.r - rhs.r)) +
                Square(static_cast<float>(lhs.g - rhs.g)) +
                Square(static_cast<float>(lhs.b - rhs.b))
                ;
        }
    }


    Palette::Palette(const std::string& n, const std::vector<rgbi>& c)
        : name(n)
        , colors(c)
    {
    }


    Palette
    Palette::Empty(const std::string& name)
    {
        return Palette{name};
    }


    const rgbi&
    Palette::GetRandomColor(Random* r) const
    {
        return r->Next(colors);
    }


    const rgbi&
    Palette::GetSafeIndex(unsigned int i) const
    {
        return colors[i % colors.size()];
    }


    const rgbi&
    Palette::GetClosestColor(const rgbi& c) const
    {
        return colors[GetIndexClosest(c)];
    }


    unsigned int
    Palette::GetIndexClosest(const rgbi& c) const
    {
        ASSERT(!colors.empty());
        auto diff_best = Diff(c, colors[0]);

        unsigned int index_best = 0;
        const auto size = colors.size();
        for(unsigned int index = 1; index < size; index += 1)
        {
            const auto diff = Diff(c, colors[index]);
            if(diff < diff_best)
            {
                diff_best = diff;
                index_best = index;
            }
        }

        return index_best;
    }


    Palette
    Palette::Rainbow(int count, float saturation, float lightness)
    {
        return Rainbow
        (
            count,
            Angle::FromRadians(0),
            Angle::FromPercentOf360
            (
                Max(1.0f, static_cast<float>(count - 1) / static_cast<float>(count))
            ),
            saturation,
            lightness
        );
    }


    Palette
    Palette::Rainbow
    (
        int count,
        const Angle& from,
        const Angle& to,
        float saturation,
        float lightness
    )
    {
        ASSERT(count > 1);

        auto pal = Palette::Empty("Rainbow");

        for(int i = 0; i < count; i += 1)
        {
            float d = static_cast<float>(i) / static_cast<float>(count - 1);
            const auto rgbf = crgb
            (
                hsl
                {
                    AngleTransform::Transform(from, d, to),
                    saturation,
                    lightness
                }
            );
            pal.colors.push_back(crgbi(rgbf));
        }

        return pal;
    }


    Palette::Palette(const std::string& n)
        : name(n)
    {
    }


    ////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        rgbi
        C(unsigned int hex)
        {
            return rgbi::from_hex(hex);
        }
    }


    namespace palette
    {
        const Palette&
        Dawnbringer()
        {
            static const auto p = Palette
            {
                "dawnbringer",
                C(0x140C1C),
                C(0x442434),
                C(0x30346D),
                C(0x4E4A4E),
                C(0x854C30),
                C(0x346524),
                C(0xD04648),
                C(0x757161),
                C(0x597DCE),
                C(0xD27D2C),
                C(0x8595A1),
                C(0x6DAA2C),
                C(0xD2AA99),
                C(0x6DC2CA),
                C(0xDAD45E),
                C(0xDEEED6),
            };
            return p;
        }


        Palette
        BuildNamedColors()
        {
            auto p = Palette
            {
                "named",
                Color::White,
                Color::LightGray,
                Color::Gray,
                Color::DarkGray,
                Color::Black,
                Color::Red,
                Color::PureRed,
                Color::Blue,
                Color::PureBlue,
                Color::LightBlue,
                Color::NormalBlue,
                Color::CornflowerBlue,
                Color::Green,
                Color::PureGreen,
                Color::LightGreen,
                Color::Yellow,
                Color::PureYellow,
                Color::Orange,
                Color::PureOrange,
                Color::Brown,
                Color::PureBrown,
                Color::Purple,
                Color::PurePurple,
                Color::Pink,
                Color::PurePink,
                Color::PureBeige,
                Color::Tan,
                Color::PureTan,
                Color::Cyan,
                Color::PureCyan,
            };
            return p;
        }


        const Palette&
        NamedColors()
        {
            static const auto p = BuildNamedColors();
            return p;
        }
    }
}

