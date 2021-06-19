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
    rgb::rgb(float red, float green, float blue) : r(red), g(green), b(blue) {}

    rgb::rgb(float gray) : r(gray), g(gray), b(gray) {}

    rgb::rgb(color color) : rgb(rgb::from_hex(colorutil::to_color_hex(color))) {}

    rgb
    rgb::from_hex(unsigned int hex)
    {
        const auto b = colorutil::to_float(colorutil::get_blue(hex));
        const auto g = colorutil::to_float(colorutil::get_green(hex));
        const auto r = colorutil::to_float(colorutil::get_red(hex));
        return rgb {r, g, b};
    }


    float
    rgb::get_length() const
    {
        return sqrt(get_length_squared());
    }

    float
    rgb::get_length_squared() const
    {
        return square(r) + square(g) + square(b);
    }


    float
    rgb::calc_luminance() const
    {
        return 0.2126f*r + 0.7152f*b + 0.0722f*b;
    }

    void
    rgb::operator+=(const rgb& rhs)
    {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
    }


    void
    rgb::operator-=(const rgb& rhs)
    {
        r -= rhs.r;
        g -= rhs.g;
        b -= rhs.b;
    }


    void
    rgb::operator*=(const rgb& rhs)
    {
        r *= rhs.r;
        g *= rhs.g;
        b *= rhs.b;
    }


    void
    rgb::operator/=(float rhs)
    {
        r /= rhs;
        g /= rhs;
        b /= rhs;
    }


    rgb
    operator+(const rgb& lhs, const rgb& rhs)
    {
        auto r = lhs;
        r += rhs;
        return r;
    }

    rgb
    operator-(const rgb& lhs, const rgb& rhs)
    {
        auto r = lhs;
        r -= rhs;
        return r;
    }

    rgb
    operator*(const rgb& lhs, const rgb& rhs)
    {
        auto r = lhs;
        r *= rhs;
        return r;
    }

    rgb
    operator/(const rgb& lhs, float rhs)
    {
        auto r = lhs;
        r /= rhs;
        return r;
    }


    rgb
    operator*(const rgb& lhs, float rhs)
    {
        return {lhs.r*rhs, lhs.g*rhs, lhs.b*rhs};
    }


    rgb
    operator*(float lhs, const rgb& rhs)
    {
        return {rhs.r*lhs, rhs.g*lhs, rhs.b*lhs};
    }




    float
    dot(const rgb& lhs, const rgb& rhs)
    {
        return lhs.r * rhs.r + lhs.g * rhs.g + lhs.b * rhs.b;
    }

    rgb
    clamp(const rgb& c)
    {
        return rgb(
                keep_within(r01, c.r),
                keep_within(r01, c.g),
                keep_within(r01, c.b));
    }


    ////////////////////////////////////////////////////////////////////////////////

    rgba::rgba(const rgb& rgb, float alpha)
        : r(rgb.r), g(rgb.g), b(rgb.b), a(alpha)
    {}

    ////////////////////////////////////////////////////////////////////////////////

#define IV(x) static_cast<int>(x)

    std::ostream&
    operator<<(std::ostream& stream, const rgbi& v)
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
    operator<<(std::ostream& stream, const rgbai& v)
    {
        return stream << "(" << IV(v.r) << ", " << IV(v.g) << ", " << IV(v.b)
                      << ", " << IV(v.a) << ")";
    }

#undef IV

    std::ostream&
    operator<<(std::ostream& stream, const rgb& v)
    {
        return stream << "(" << v.r << ", " << v.g << ", " << v.b << ")";
    }


    std::ostream&
    operator<<(std::ostream& stream, const rgba& v)
    {
        return stream << "(" << v.r << ", " << v.g << ", " << v.b << ", " << v.a
                      << ")";
    }

    std::ostream&
    operator<<(std::ostream& stream, const hsl& v)
    {
        return stream << "(" << v.h.in_degrees() << "°, " << v.s * 100 << "%, "
                      << v.l * 100 << "%)";
    }


    ////////////////////////////////////////////////////////////////////////////////
    // Default compare

    bool
    operator==(const rgbi& lhs, const rgbi& rhs)
    {
        return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
    }


    bool
    operator!=(const rgbi& lhs, const rgbi& rhs)
    {
        return !(lhs == rhs);
    }


    bool
    operator==(const rgbai& lhs, const rgbai& rhs)
    {
        return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
    }


    bool
    operator!=(const rgbai& lhs, const rgbai& rhs)
    {
        return !(lhs == rhs);
    }

    ////////////////////////////////////////////////////////////////////////////////

    rgb
    crgb(const rgbi& rgb)
    {
        return {colorutil::to_float(rgb.r),
                colorutil::to_float(rgb.g),
                colorutil::to_float(rgb.b)};
    }

    rgb
    crgb(const rgba& rgb)
    {
        return {rgb.r, rgb.g, rgb.b};
    }

    rgb
    crgb(const rgbai& rgb)
    {
        return {colorutil::to_float(rgb.r),
                colorutil::to_float(rgb.g),
                colorutil::to_float(rgb.b)};
    }

    rgb
    crgb(const hsl& hsl)
    {
        // based on https://gist.github.com/mjackson/5311256
        if(hsl.s == 0)
        {
            return rgb {hsl.l}; // achromatic
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

    hsl
    chsl(const rgb& c)
    {
        // based on https://gist.github.com/mjackson/5311256
        const auto max = core::max(c.r, core::max(c.g, c.b));
        const auto min = core::min(c.r, core::min(c.g, c.b));
        const auto l = (max + min) / 2;
        // var h, s;

        enum class biggest_value
        {
            r,
            g,
            b,
            same
        };

        const auto cl = [](float r, float g, float b) -> biggest_value {
            constexpr auto min_diff = 0.001f;
            if(abs(r - g) < min_diff && abs(g - b) < min_diff)
            {
                return biggest_value::same;
            }
            if(r >= g && r >= b)
            {
                return biggest_value::r;
            }
            if(g >= r && g >= b)
            {
                return biggest_value::g;
            }
            ASSERTX(b >= r && b >= g, r, g, b);
            return biggest_value::b;
        }(c.r, c.g, c.b);

        if(cl == biggest_value::same)
        {
            return {angle::from_radians(0), 0, l}; // achromatic
        }
        else
        {
            const auto d = max - min;
            const auto s = l > 0.5 ? d / (2 - max - min) : d / (max + min);

            const float h = [cl, &c, d]() -> float
            {
                switch(cl)
                {
                case biggest_value::r: return (c.g - c.b) / d + (c.g < c.b ? 6.0f : 0.0f);
                case biggest_value::g: return (c.b - c.r) / d + 2;
                case biggest_value::b: return (c.r - c.g) / d + 4;
                default: DIE("Unreachable"); return 0.0f;
                }
            }() / 6;
            return {angle::from_percent_of_360(h), s, l};
        }
    }

    // Convert functions (rgbi)

    rgbi
    crgbi(const rgb& rgb)
    {
        return
        {
            colorutil::to_unsigned_char(rgb.r),
            colorutil::to_unsigned_char(rgb.g),
            colorutil::to_unsigned_char(rgb.b)
        };
    }

    rgbi
    crgbi(const rgba& rgb)
    {
        return
        {
            colorutil::to_unsigned_char(rgb.r),
            colorutil::to_unsigned_char(rgb.g),
            colorutil::to_unsigned_char(rgb.b)
        };
    }

    rgbi
    crgbi(const rgbai& rgb)
    {
        return {rgb.r, rgb.g, rgb.b};
    }

    //
    rgba
    crgba(const rgbai& rgb)
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

    rgbai
    crgbai(const rgba& rgb)
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

    rgb
    rgb_transform::Transform(const rgb& from, float v, const rgb& to)
    {
        return
        {
            FloatTransform::Transform(from.r, v, to.r),
            FloatTransform::Transform(from.g, v, to.g),
            FloatTransform::Transform(from.b, v, to.b)
        };
    }

    ////////////////////////////////////////////////////////////////////////////////

    hsl
    saturate(const hsl& ahsl, float amount, is_absolute method)
    {
        auto hsl = ahsl;

        if(method == is_absolute::no)
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

    hsl
    desaturate(const hsl& ahsl, float amount, is_absolute method)
    {
        auto hsl = ahsl;

        if(method == is_absolute::no)
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

    hsl
    lighten(const hsl& ahsl, float amount, is_absolute method)
    {
        auto hsl = ahsl;

        if(method == is_absolute::no)
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

    hsl
    darken(const hsl& ahsl, float amount, is_absolute method)
    {
        auto hsl = ahsl;

        if(method == is_absolute::no)
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

    rgb
    shade_color(const rgb& color, float percentage)
    {
        // //
        // https://stackoverflow.com/questions/5560248/programmatically-lighten-or-darken-a-hex-color-or-rgb-and-blend-colors
        const float t = percentage < 0 ? 0.0f : 1.0f;
        const float p = percentage < 0 ? -percentage : percentage;
        const float r = (t - color.r) * p + color.r;
        const float g = (t - color.g) * p + color.g;
        const float b = (t - color.b) * p + color.b;
        return rgb{r, g, b};
    }

    ////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        // parses a #fff or a #ffffff string as a color
        result<rgbi>
        parse_hash_hex_rgbi(const std::string& value)
        {
            using R = result<rgbi>;

            auto parse_rgb_hex = [&](int size) -> R
            {
                auto parse_hex = [&](int index, int len) ->
                    std::pair<std::optional<std::uint8_t>, std::string>
                {
                    const auto s = value.substr(1+index*len, len);
                    auto ss = std::istringstream(s);
                    int hex = 0;
                    ss >> std::hex >> hex;
                    if(ss.eof() && ss.fail()==false)
                    { return {static_cast<std::uint8_t>(hex), s}; }
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
                    if(!r) { invalids.emplace_back(string_builder{} << "red(" << r_value << ")"); }
                    if(!g) { invalids.emplace_back(string_builder{} << "green(" << g_value << ")"); }
                    if(!b) { invalids.emplace_back(string_builder{} << "blue(" << b_value << ")"); }
                    return R::create_error
                    (
                        string_builder() << "#color contains invalid hex for " <<
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
                    string_builder() << "a hexadecimal color needs to be either #abc "
                    "or #aabbcc. current count: " << (size-1)
                );
            }
        }
    }

    [[nodiscard]]
    result<rgbi>
    crgbi(const std::string& original_value)
    {
        using R = result<rgbi>;
        const auto value = trim(original_value);

        if(value.empty()) { return R::create_error("empty string is not a color");}

        if(value[0] == '#')
        {
            return parse_hash_hex_rgbi(value);
        }
        else
        {
            const auto match = string_to_enum<color>(value);
            if(match.single_match) { return R::create_value(crgbi(match.values[0])); }
            return R::create_error
            (
                string_builder() << "bad name. Hex values require a #, but it could also be either " <<
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
                s = string_builder() << s[0] << s[0] << s[1] << s[1] << s[2] << s[2];
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
