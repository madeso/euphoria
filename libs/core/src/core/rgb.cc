#include "core/rgb.h"

#include <iostream>
#include <map>
#include <iomanip>

#include "core/interpolate.h"
#include "core/numeric.h"
#include "core/stringutils.h"
#include "core/range.h"
#include "core/stringmerger.h"


namespace euphoria::core
{
    Rgb::Rgb(float red, float green, float blue) : r(red), g(green), b(blue) {}

    Rgb::Rgb(float gray) : r(gray), g(gray), b(gray) {}

    Rgb::Rgb(Color color) : Rgb(Rgb::FromHex(colorutil::ToColorHex(color))) {}

    Rgb
    Rgb::FromHex(unsigned int hex)
    {
        const auto b = colorutil::ToFloat(colorutil::GetBlue(hex));
        const auto g = colorutil::ToFloat(colorutil::GetGreen(hex));
        const auto r = colorutil::ToFloat(colorutil::GetRed(hex));
        return Rgb {r, g, b};
    }


    float
    Rgb::GetLength() const
    {
        return Sqrt(GetLengthSquared());
    }

    float
    Rgb::GetLengthSquared() const
    {
        return Square(r) + Square(g) + Square(b);
    }


    float
    Rgb::CalcLuminance() const
    {
        return 0.2126f*r + 0.7152f*b + 0.0722f*b;
    }

    void
    Rgb::operator+=(const Rgb& rhs)
    {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
    }


    void
    Rgb::operator-=(const Rgb& rhs)
    {
        r -= rhs.r;
        g -= rhs.g;
        b -= rhs.b;
    }


    void
    Rgb::operator*=(const Rgb& rhs)
    {
        r *= rhs.r;
        g *= rhs.g;
        b *= rhs.b;
    }


    void
    Rgb::operator/=(float rhs)
    {
        r /= rhs;
        g /= rhs;
        b /= rhs;
    }


    Rgb
    operator+(const Rgb& lhs, const Rgb& rhs)
    {
        auto r = lhs;
        r += rhs;
        return r;
    }

    Rgb
    operator-(const Rgb& lhs, const Rgb& rhs)
    {
        auto r = lhs;
        r -= rhs;
        return r;
    }

    Rgb
    operator*(const Rgb& lhs, const Rgb& rhs)
    {
        auto r = lhs;
        r *= rhs;
        return r;
    }

    Rgb
    operator/(const Rgb& lhs, float rhs)
    {
        auto r = lhs;
        r /= rhs;
        return r;
    }


    Rgb
    operator*(const Rgb& lhs, float rhs)
    {
        return {lhs.r*rhs, lhs.g*rhs, lhs.b*rhs};
    }


    Rgb
    operator*(float lhs, const Rgb& rhs)
    {
        return {rhs.r*lhs, rhs.g*lhs, rhs.b*lhs};
    }




    float
    dot(const Rgb& lhs, const Rgb& rhs)
    {
        return lhs.r * rhs.r + lhs.g * rhs.g + lhs.b * rhs.b;
    }

    Rgb
    Clamp(const Rgb& c)
    {
        return Rgb(
                KeepWithin(R01(), c.r),
                KeepWithin(R01(), c.g),
                KeepWithin(R01(), c.b));
    }


    ////////////////////////////////////////////////////////////////////////////////

    Rgba::Rgba(const Rgb& rgb, float alpha)
        : r(rgb.r), g(rgb.g), b(rgb.b), a(alpha)
    {}

    ////////////////////////////////////////////////////////////////////////////////

    const Angle&
    Hsl::Red()
    {
        static const Angle a = Angle::FromPercentOf360(0.0f);
        return a;
    }

    const Angle&
    Hsl::Yellow()
    {
        static const Angle a = Angle::FromPercentOf360(1 / 6.0f);
        return a;
    }

    const Angle&
    Hsl::Green()
    {
        static const Angle a = Angle::FromPercentOf360(2 / 6.0f);
        return a;
    }

    const Angle&
    Hsl::Cyan()
    {
        static const Angle a = Angle::FromPercentOf360(3 / 6.0f);
        return a;
    }

    const Angle&
    Hsl::Blue()
    {
        static const Angle a = Angle::FromPercentOf360(4 / 6.0f);
        return a;
    }

    const Angle&
    Hsl::Magenta()
    {
        static const Angle a = Angle::FromPercentOf360(5 / 6.0f);
        return a;
    }

    ////////////////////////////////////////////////////////////////////////////////

#define IV(x) static_cast<int>(x)

    std::ostream&
    operator<<(std::ostream& stream, const Rgbi& v)
    {
        const auto flags = std::ios_base::fmtflags {stream.flags()};
        stream << "#" << std::hex
            << std::setfill('0') << std::setw(2) << +v.r
            << std::setfill('0') << std::setw(2) << +v.g
            << std::setfill('0') << std::setw(2) << +v.b;
        stream.flags(flags);
        return stream;
    }


    std::ostream&
    operator<<(std::ostream& stream, const Rgbai& v)
    {
        return stream << "(" << IV(v.r) << ", " << IV(v.g) << ", " << IV(v.b)
                      << ", " << IV(v.a) << ")";
    }

#undef IV

    std::ostream&
    operator<<(std::ostream& stream, const Rgb& v)
    {
        return stream << "(" << v.r << ", " << v.g << ", " << v.b << ")";
    }


    std::ostream&
    operator<<(std::ostream& stream, const Rgba& v)
    {
        return stream << "(" << v.r << ", " << v.g << ", " << v.b << ", " << v.a
                      << ")";
    }

    std::ostream&
    operator<<(std::ostream& stream, const Hsl& v)
    {
        return stream << "(" << v.h.InDegrees() << "°, " << v.s * 100 << "%, "
                      << v.l * 100 << "%)";
    }


    ////////////////////////////////////////////////////////////////////////////////
    // Default compare

    bool
    operator==(const Rgbi& lhs, const Rgbi& rhs)
    {
        return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
    }


    bool
    operator!=(const Rgbi& lhs, const Rgbi& rhs)
    {
        return !(lhs == rhs);
    }


    bool
    operator==(const Rgbai& lhs, const Rgbai& rhs)
    {
        return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
    }


    bool
    operator!=(const Rgbai& lhs, const Rgbai& rhs)
    {
        return !(lhs == rhs);
    }

    ////////////////////////////////////////////////////////////////////////////////

    Rgb
    rgb(const Rgbi& rgb)
    {
        return {colorutil::ToFloat(rgb.r),
                colorutil::ToFloat(rgb.g),
                colorutil::ToFloat(rgb.b)};
    }

    Rgb
    rgb(const Rgba& rgb)
    {
        return {rgb.r, rgb.g, rgb.b};
    }

    Rgb
    rgb(const Rgbai& rgb)
    {
        return {colorutil::ToFloat(rgb.r),
                colorutil::ToFloat(rgb.g),
                colorutil::ToFloat(rgb.b)};
    }

    Rgb
    rgb(const Hsl& hsl)
    {
        // based on https://gist.github.com/mjackson/5311256
        if(hsl.s == 0)
        {
            return Rgb {hsl.l};  // achromatic
        }
        else
        {
            auto hue2rgb = [](float p, float q, float t) {
                if(t < 0.0f)
                    t += 1.0f;
                if(t > 1.0f)
                    t -= 1.0f;
                if(t < 1.0f / 6.0f)
                    return p + (q - p) * 6.0f * t;
                if(t < 1.0f / 2.0f)
                    return q;
                if(t < 2.0f / 3.0f)
                    return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
                return p;
            };

            const auto q = hsl.l < 0.5f ? hsl.l * (1.0f + hsl.s)
                                        : hsl.l + hsl.s - hsl.l * hsl.s;
            const auto p = 2.0f * hsl.l - q;

            const auto r = hue2rgb(p, q, hsl.h.InPercentOf360() + 1.0f / 3.0f);
            const auto g = hue2rgb(p, q, hsl.h.InPercentOf360());
            const auto b = hue2rgb(p, q, hsl.h.InPercentOf360() - 1.0f / 3.0f);
            return {r, g, b};
        }
    }

    // Convert functions (hsl)

    Hsl
    hsl(const Rgb& c)
    {
        // based on https://gist.github.com/mjackson/5311256
        const auto max = Max(c.r, Max(c.g, c.b));
        const auto min = Min(c.r, Min(c.g, c.b));
        const auto l   = (max + min) / 2;
        // var        h, s;

        enum class Biggest
        {
            Red,
            Green,
            Blue,
            Same
        };

        auto cl = [](float r, float g, float b) -> Biggest {
            constexpr auto min_diff = 0.001f;
            if(Abs(r - g) < min_diff && Abs(g - b) < min_diff)
            {
                return Biggest::Same;
            }
            if(r >= g && r >= b)
            {
                return Biggest::Red;
            }
            if(g >= r && g >= b)
            {
                return Biggest::Green;
            }
            ASSERTX(b >= r && b >= g, r, g, b);
            return Biggest::Blue;
        }(c.r, c.g, c.b);

        if(cl == Biggest::Same)
        {
            return {Angle::FromRadians(0), 0, l};  // achromatic
        }
        else
        {
            const auto d = max - min;
            const auto s = l > 0.5 ? d / (2 - max - min) : d / (max + min);

            float h = 0;
            switch(cl)
            {
            case Biggest::Red: h = (c.g - c.b) / d + (c.g < c.b ? 6 : 0); break;
            case Biggest::Green: h = (c.b - c.r) / d + 2; break;
            case Biggest::Blue: h = (c.r - c.g) / d + 4; break;
            default: h = 0; break;
            }

            h /= 6;
            return {Angle::FromPercentOf360(h), s, l};
        }
    }

    // Convert functions (rgbi)

    Rgbi
    rgbi(const Rgb& rgb)
    {
        return {colorutil::ToUnsignedChar(rgb.r),
                colorutil::ToUnsignedChar(rgb.g),
                colorutil::ToUnsignedChar(rgb.b)};
    }

    Rgbi
    rgbi(const Rgba& rgb)
    {
        return {colorutil::ToUnsignedChar(rgb.r),
                colorutil::ToUnsignedChar(rgb.g),
                colorutil::ToUnsignedChar(rgb.b)};
    }

    Rgbi
    rgbi(const Rgbai& rgb)
    {
        return {rgb.r, rgb.g, rgb.b};
    }

    //
    Rgba
    rgba(const Rgbai& rgb)
    {
        return
        {
            {
                colorutil::ToFloat(rgb.r),
                colorutil::ToFloat(rgb.g),
                colorutil::ToFloat(rgb.b),
            },
            colorutil::ToFloat(rgb.a)
        };
    }

    Rgbai
    rgbai(const Rgba& rgb)
    {
        return
        {
            {
                colorutil::ToUnsignedChar(rgb.r),
                colorutil::ToUnsignedChar(rgb.g),
                colorutil::ToUnsignedChar(rgb.b),
            },
            colorutil::ToUnsignedChar(rgb.a)
        };
    }

    ////////////////////////////////////////////////////////////////////////////////

    Rgb
    RgbTransform::Transform(const Rgb& from, float v, const Rgb& to)
    {
        return {FloatTransform::Transform(from.r, v, to.r),
                FloatTransform::Transform(from.g, v, to.g),
                FloatTransform::Transform(from.b, v, to.b)};
    }

    ////////////////////////////////////////////////////////////////////////////////

    Hsl
    saturate(const Hsl& ahsl, float amount, Method method)
    {
        auto hsl = ahsl;

        if(method == Method::Relative)
        {
            hsl.s += hsl.s * amount;
        }
        else
        {
            hsl.s += amount;
        }
        hsl.s = KeepWithin(R01(), hsl.s);
        return hsl;
    }

    Hsl
    desaturate(const Hsl& ahsl, float amount, Method method)
    {
        auto hsl = ahsl;

        if(method == Method::Relative)
        {
            hsl.s -= hsl.s * amount;
        }
        else
        {
            hsl.s -= amount;
        }
        hsl.s = KeepWithin(R01(), hsl.s);
        return hsl;
    }

    Hsl
    lighten(const Hsl& ahsl, float amount, Method method)
    {
        auto hsl = ahsl;

        if(method == Method::Relative)
        {
            hsl.l += hsl.l * amount;
        }
        else
        {
            hsl.l += amount;
        }
        hsl.l = KeepWithin(R01(), hsl.l);
        return hsl;
    }

    Hsl
    darken(const Hsl& ahsl, float amount, Method method)
    {
        auto hsl = ahsl;

        if(method == Method::Relative)
        {
            hsl.l -= hsl.l * amount;
        }
        else
        {
            hsl.l -= amount;
        }
        hsl.l = KeepWithin(R01(), hsl.l);
        return hsl;
    }

    Rgb
    ShadeColor(const Rgb& rgb, float percentage)
    {
        // //
        // https://stackoverflow.com/questions/5560248/programmatically-lighten-or-darken-a-hex-color-or-rgb-and-blend-colors
        const float t = percentage < 0 ? 0.0f : 1.0f;
        const float p = percentage < 0 ? -percentage : percentage;
        const float r = (t - rgb.r) * p + rgb.r;
        const float g = (t - rgb.g) * p + rgb.g;
        const float b = (t - rgb.b) * p + rgb.b;
        return Rgb {r, g, b};
    }

    ////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        // parses a #fff or a #ffffff string as a color
        Result<Rgbi>
        ParseHashHexRgbi(const std::string& value)
        {
            using R = Result<Rgbi>;

            auto parse_rgb_hex = [&](int size) -> R
            {
                auto parse_hex = [&](int index, int len) ->
                    std::pair<std::optional<std::uint8_t>, std::string>
                {
                    const auto s = value.substr(1+index*len, len);
                    auto ss = std::istringstream(s);
                    int hex;
                    ss >> std::hex >> hex;
                    if(ss.eof() && ss.fail()==false)
                    { return {static_cast<std::uint8_t>(hex), s}; }
                    else return {std::nullopt, s};
                };

                const auto [r, r_value] = parse_hex(0, size);
                const auto [g, g_value] = parse_hex(1, size);
                const auto [b, b_value] = parse_hex(2, size);
                if(r && r && b)
                {
                    return R::True({*r, *g, *b});
                }
                else
                {
                    auto invalids = std::vector<std::string>{};
                    if(!r) { invalids.emplace_back(Str{} << "red(" << r_value << ")"); }
                    if(!g) { invalids.emplace_back(Str{} << "green(" << g_value << ")"); }
                    if(!b) { invalids.emplace_back(Str{} << "blue(" << b_value << ")"); }
                    return R::False
                    (
                        Str() << "#color contains invalid hex for " <<
                        StringMerger::EnglishAnd().Generate(invalids)
                    );
                }
            };

            const auto size = value.length();
            switch(size)
            {
            case 4: return parse_rgb_hex(1);
            case 7: return parse_rgb_hex(2);
            default: return R::False
                (
                    Str() << "a hexadecimal color needs to be either #abc "
                    "or #aabbcc. current count: " << (size-1)
                );
            }
        }
    }

    [[nodiscard]]
    Result<Rgbi>
    rgbi(const std::string& original_value)
    {
        using R = Result<Rgbi>;
        const auto value = Trim(original_value);

        if(value.empty()) { return R::False("empty string is not a color");}

        if(value[0] == '#')
        {
            return ParseHashHexRgbi(value);
        }
        else
        {
            const auto match = StringToEnum<Color>(value);
            if(match.single_match) { return R::True(rgbi(match.values[0])); }
            return R::False
            (
                Str() << "bad name. Hex values require a #, but it could also be either " <<
                StringMerger::EnglishOr().Generate
                (
                    match.names
                )
            );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////

    namespace colorutil
    {

        unsigned int
        FromStringToHex(const std::string& str)
        {
            auto s = Trim(str);
            ASSERT(!s.empty());
            s = s[0] == '#' ? ToLower(s.substr(1)) : ToLower(s);
            if(s.length() == 3)
            {
                s = Str() << s[0] << s[0] << s[1] << s[1] << s[2] << s[2];
            }
            if(s.length() != 6)
                return 0;
            std::istringstream ss {s};
            unsigned int       hex = 0;
            ss >> std::hex >> hex;
            ASSERTX(!ss.fail(), s, str, hex);
            return hex;
        }
    }  // namespace colorutil

}  // namespace euphoria::core