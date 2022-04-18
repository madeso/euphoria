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

    Rgb::Rgb(NamedColor color) : Rgb(Rgb::from_hex(colorutil::to_color_hex(color))) {}

    Rgb
    Rgb::from_hex(unsigned int hex)
    {
        const auto b = colorutil::to_float(colorutil::get_blue(hex));
        const auto g = colorutil::to_float(colorutil::get_green(hex));
        const auto r = colorutil::to_float(colorutil::get_red(hex));
        return Rgb {r, g, b};
    }


    float
    Rgb::get_length() const
    {
        return sqrt(get_length_squared());
    }

    float
    Rgb::get_length_squared() const
    {
        return square(r) + square(g) + square(b);
    }


    float
    Rgb::calc_luminance() const
    {
        // 0.3 0.59 0.11?
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
    clamp(const Rgb& c)
    {
        return Rgb(
                keep_within(r01, c.r),
                keep_within(r01, c.g),
                keep_within(r01, c.b));
    }


    ////////////////////////////////////////////////////////////////////////////////

    Rgba::Rgba(const Rgb& rgb, float alpha)
        : r(rgb.r), g(rgb.g), b(rgb.b), a(alpha)
    {}

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
        return stream << "(" << v.h.in_degrees() << "°, " << v.s * 100 << "%, "
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
    crgb(const Rgbi& rgb)
    {
        return {colorutil::to_float(rgb.r),
                colorutil::to_float(rgb.g),
                colorutil::to_float(rgb.b)};
    }

    Rgb
    crgb(const Rgba& rgb)
    {
        return {rgb.r, rgb.g, rgb.b};
    }

    Rgb
    crgb(const Rgbai& rgb)
    {
        return {colorutil::to_float(rgb.r),
                colorutil::to_float(rgb.g),
                colorutil::to_float(rgb.b)};
    }

    Rgb
    crgb(const Hsl& hsl)
    {
        // based on https://gist.github.com/mjackson/5311256
        if(hsl.s == 0)
        {
            return Rgb {hsl.l}; // achromatic
        }
        else
        {
            auto hue2rgb = [](float p, float q, float t) {
                if(t < 0.0f) { t += 1.0f; }
                if(t > 1.0f) { t -= 1.0f; }

                if(t < 1.0f / 6.0f) { return p + (q - p) * 6.0f * t; }
                else if(t < 1.0f / 2.0f) { return q; }
                else if(t < 2.0f / 3.0f) { return p + (q - p) * (2.0f / 3.0f - t) * 6.0f; }
                else { return p; }
            };

            const auto q = hsl.l < 0.5f ? hsl.l * (1.0f + hsl.s)
                                        : hsl.l + hsl.s - hsl.l * hsl.s;
            const auto p = 2.0f * hsl.l - q;

            const auto r = hue2rgb(p, q, hsl.h.in_percent_of_360() + 1.0f / 3.0f);
            const auto g = hue2rgb(p, q, hsl.h.in_percent_of_360());
            const auto b = hue2rgb(p, q, hsl.h.in_percent_of_360() - 1.0f / 3.0f);
            return {r, g, b};
        }
    }

    // Convert functions (hsl)

    Hsl
    chsl(const Rgb& c)
    {
        // based on https://gist.github.com/mjackson/5311256
        const auto max = core::max(c.r, core::max(c.g, c.b));
        const auto min = core::min(c.r, core::min(c.g, c.b));
        const auto l = (max + min) / 2;
        // var h, s;

        enum class BiggestValue
        {
            r,
            g,
            b,
            same
        };

        const auto cl = [](float r, float g, float b) -> BiggestValue {
            constexpr auto min_diff = 0.001f;
            if(abs(r - g) < min_diff && abs(g - b) < min_diff)
            {
                return BiggestValue::same;
            }
            if(r >= g && r >= b)
            {
                return BiggestValue::r;
            }
            if(g >= r && g >= b)
            {
                return BiggestValue::g;
            }
            ASSERTX(b >= r && b >= g, r, g, b);
            return BiggestValue::b;
        }(c.r, c.g, c.b);

        if(cl == BiggestValue::same)
        {
            return {Angle::from_radians(0), 0, l}; // achromatic
        }
        else
        {
            const auto d = max - min;
            const auto s = l > 0.5f ? d / (2 - max - min) : d / (max + min);

            const float h = [cl, &c, d]() -> float
            {
                switch(cl)
                {
                case BiggestValue::r: return (c.g - c.b) / d + (c.g < c.b ? 6.0f : 0.0f);
                case BiggestValue::g: return (c.b - c.r) / d + 2;
                case BiggestValue::b: return (c.r - c.g) / d + 4;
                default: DIE("Unreachable"); return 0.0f;
                }
            }() / 6;
            return {Angle::from_percent_of_360(h), s, l};
        }
    }

    // Convert functions (rgbi)

    Rgbi
    crgbi(const Rgb& rgb)
    {
        return
        {
            colorutil::to_unsigned_char(rgb.r),
            colorutil::to_unsigned_char(rgb.g),
            colorutil::to_unsigned_char(rgb.b)
        };
    }

    Rgbi
    crgbi(const Rgba& rgb)
    {
        return
        {
            colorutil::to_unsigned_char(rgb.r),
            colorutil::to_unsigned_char(rgb.g),
            colorutil::to_unsigned_char(rgb.b)
        };
    }

    Rgbi
    crgbi(const Rgbai& rgb)
    {
        return {rgb.r, rgb.g, rgb.b};
    }

    //
    Rgba
    crgba(const Rgbai& rgb)
    {
        return
        {
            {
                colorutil::to_float(rgb.r),
                colorutil::to_float(rgb.g),
                colorutil::to_float(rgb.b),
            },
            colorutil::to_float(rgb.a)
        };
    }

    Rgbai
    crgbai(const Rgba& rgb)
    {
        return
        {
            {
                colorutil::to_unsigned_char(rgb.r),
                colorutil::to_unsigned_char(rgb.g),
                colorutil::to_unsigned_char(rgb.b),
            },
            colorutil::to_unsigned_char(rgb.a)
        };
    }

    ////////////////////////////////////////////////////////////////////////////////

    Rgb
    RgbTransform::transform(const Rgb& from, float v, const Rgb& to)
    {
        return
        {
            FloatTransform::transform(from.r, v, to.r),
            FloatTransform::transform(from.g, v, to.g),
            FloatTransform::transform(from.b, v, to.b)
        };
    }

    ////////////////////////////////////////////////////////////////////////////////

    Hsl
    saturate(const Hsl& ahsl, float amount, IsAbsolute method)
    {
        auto hsl = ahsl;

        if(method == IsAbsolute::no)
        {
            hsl.s += hsl.s * amount;
        }
        else
        {
            hsl.s += amount;
        }
        hsl.s = keep_within(r01, hsl.s);
        return hsl;
    }

    Hsl
    desaturate(const Hsl& ahsl, float amount, IsAbsolute method)
    {
        auto hsl = ahsl;

        if(method == IsAbsolute::no)
        {
            hsl.s -= hsl.s * amount;
        }
        else
        {
            hsl.s -= amount;
        }
        hsl.s = keep_within(r01, hsl.s);
        return hsl;
    }

    Hsl
    lighten(const Hsl& ahsl, float amount, IsAbsolute method)
    {
        auto hsl = ahsl;

        if(method == IsAbsolute::no)
        {
            hsl.l += hsl.l * amount;
        }
        else
        {
            hsl.l += amount;
        }
        hsl.l = keep_within(r01, hsl.l);
        return hsl;
    }

    Hsl
    darken(const Hsl& ahsl, float amount, IsAbsolute method)
    {
        auto hsl = ahsl;

        if(method == IsAbsolute::no)
        {
            hsl.l -= hsl.l * amount;
        }
        else
        {
            hsl.l -= amount;
        }
        hsl.l = keep_within(r01, hsl.l);
        return hsl;
    }

    Rgb
    shade_color(const Rgb& color, float percentage)
    {
        // //
        // https://stackoverflow.com/questions/5560248/programmatically-lighten-or-darken-a-hex-color-or-rgb-and-blend-colors
        const float t = percentage < 0 ? 0.0f : 1.0f;
        const float p = percentage < 0 ? -percentage : percentage;
        const float r = (t - color.r) * p + color.r;
        const float g = (t - color.g) * p + color.g;
        const float b = (t - color.b) * p + color.b;
        return Rgb{r, g, b};
    }

    ////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        // parses a #fff or a #ffffff string as a color
        Result<Rgbi>
        parse_hash_hex_rgbi(const std::string& value)
        {
            using R = Result<Rgbi>;

            auto parse_rgb_hex = [&](int size) -> R
            {
                auto parse_hex = [&](int index, int len) ->
                    std::pair<std::optional<U8>, std::string>
                {
                    const auto s = value.substr(1+index*len, len);
                    auto ss = std::istringstream(s);
                    int hex = 0;
                    ss >> std::hex >> hex;
                    if(ss.eof() && ss.fail()==false)
                    { return {static_cast<U8>(hex), s}; }
                    else { return {std::nullopt, s}; };
                };

                const auto [r, r_value] = parse_hex(0, size);
                const auto [g, g_value] = parse_hex(1, size);
                const auto [b, b_value] = parse_hex(2, size);
                if(r && r && b)
                {
                    return R::create_value({*r, *g, *b});
                }
                else
                {
                    auto invalids = std::vector<std::string>{};
                    if(!r) { invalids.emplace_back(StringBuilder{} << "red(" << r_value << ")"); }
                    if(!g) { invalids.emplace_back(StringBuilder{} << "green(" << g_value << ")"); }
                    if(!b) { invalids.emplace_back(StringBuilder{} << "blue(" << b_value << ")"); }
                    return R::create_error
                    (
                        StringBuilder() << "#color contains invalid hex for " <<
                        string_mergers::english_and.merge(invalids)
                    );
                }
            };

            const auto size = value.length();
            switch(size)
            {
            case 4: return parse_rgb_hex(1);
            case 7: return parse_rgb_hex(2);
            default: return R::create_error
                (
                    StringBuilder() << "a hexadecimal color needs to be either #abc "
                    "or #aabbcc. current count: " << (size-1)
                );
            }
        }
    }

    [[nodiscard]]
    Result<Rgbi>
    crgbi(const std::string& original_value)
    {
        using R = Result<Rgbi>;
        const auto value = trim(original_value);

        if(value.empty()) { return R::create_error("empty string is not a color");}

        if(value[0] == '#')
        {
            return parse_hash_hex_rgbi(value);
        }
        else
        {
            const auto match = string_to_enum<NamedColor>(value);
            if(match.single_match) { return R::create_value(crgbi(match.values[0])); }
            return R::create_error
            (
                StringBuilder() << "bad name. Hex values require a #, but it could also be either " <<
                string_mergers::english_or.merge
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
        from_string_to_hex(const std::string& str)
        {
            auto s = trim(str);
            ASSERT(!s.empty());
            s = s[0] == '#' ? to_lower(s.substr(1)) : to_lower(s);
            if(s.length() == 3)
            {
                s = StringBuilder() << s[0] << s[0] << s[1] << s[1] << s[2] << s[2];
            }
            if(s.length() != 6)
            {
                return 0;
            }
            std::istringstream ss {s};
            unsigned int hex = 0;
            ss >> std::hex >> hex;
            ASSERTX(!ss.fail(), s, str, hex);
            return hex;
        }
    }

}
