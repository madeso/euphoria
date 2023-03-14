#include "core/rgb.h"

#include <iostream>
#include <map>
#include <iomanip>

#include "core/interpolate.default.h"
#include "core/numeric.h"
#include "core/stringutils.h"
#include "core/range.h"
#include "core/stringmerger.h"


namespace euphoria::core
{
    rgb::rgb(float red, float green, float blue) : r(red), g(green), b(blue) {}

    rgb::rgb(float gray) : r(gray), g(gray), b(gray) {}

    rgb::rgb(NamedColor color) : rgb(rgb::from_hex(colorutil::to_color_hex(color))) {}

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
        // 0.3 0.59 0.11?
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

    rgba::rgba(const rgb& c, float alpha)
        : r(c.r), g(c.g), b(c.b), a(alpha)
    {
    }
    

    rgba::rgba(float red, float green, float blue, float alpha)
        : r(red), g(green), b(blue), a(alpha)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////

    std::string to_string(const rgbi& c)
    {
        return "#{:0>2x}{:0>2x}{:0>2x}"_format(c.r, c.g, c.b);
    }

    std::string to_string(const rgbai& c)
    {
        return "({}, {}, {}, {})"_format(c.r, c.g, c.b, c.a);
    }

    std::string to_string(const rgb& v)
    {
        return "({}, {}, {})"_format(v.r, v.g, v.b);
    }

    std::string to_string(const rgba& v)
    {
        return "({}, {}, {}, {})"_format(v.r, v.g, v.b, v.a);
    }

    std::string to_string(const Hsl& v)
    {
        return "({:.0f}°, {:.0f}%, {:.0f}%)"_format(v.h.in_degrees(), v.s * 100, v.l * 100);
    }

    std::ostream& operator<<(std::ostream& stream, const rgbi& v)  { stream << to_string(v); return stream; }
    std::ostream& operator<<(std::ostream& stream, const rgbai& v) { stream << to_string(v); return stream; }
    std::ostream& operator<<(std::ostream& stream, const rgb& v)   { stream << to_string(v); return stream; }
    std::ostream& operator<<(std::ostream& stream, const rgba& v)  { stream << to_string(v); return stream; }
    std::ostream& operator<<(std::ostream& stream, const Hsl& v)   { stream << to_string(v); return stream; }


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
    crgb(const rgbi& c)
    {
        return {colorutil::to_float(c.r),
                colorutil::to_float(c.g),
                colorutil::to_float(c.b)};
    }

    rgb
    crgb(const rgba& c)
    {
        return {c.r, c.g, c.b};
    }

    rgb
    crgb(const rgbai& c)
    {
        return {colorutil::to_float(c.r),
                colorutil::to_float(c.g),
                colorutil::to_float(c.b)};
    }

    rgb
    crgb(const Hsl& hsl)
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

    Hsl
    chsl(const rgb& c)
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
            return {angle::from_radians(0), 0, l}; // achromatic
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
            return {angle::from_percent_of_360(h), s, l};
        }
    }

    // Convert functions (rgbi)

    rgbi
    crgbi(const rgb& c)
    {
        return
        {
            colorutil::to_unsigned_char(c.r),
            colorutil::to_unsigned_char(c.g),
            colorutil::to_unsigned_char(c.b)
        };
    }

    rgbi
    crgbi(const rgba& c)
    {
        return
        {
            colorutil::to_unsigned_char(c.r),
            colorutil::to_unsigned_char(c.g),
            colorutil::to_unsigned_char(c.b)
        };
    }

    rgbi
    crgbi(const rgbai& c)
    {
        return {c.r, c.g, c.b};
    }

    //
    rgba
    crgba(const rgbai& c)
    {
        return
        {
            {
                colorutil::to_float(c.r),
                colorutil::to_float(c.g),
                colorutil::to_float(c.b),
            },
            colorutil::to_float(c.a)
        };
    }

    rgbai
    crgbai(const rgba& c)
    {
        return
        {
            {
                colorutil::to_unsigned_char(c.r),
                colorutil::to_unsigned_char(c.g),
                colorutil::to_unsigned_char(c.b),
            },
            colorutil::to_unsigned_char(c.a)
        };
    }

    ////////////////////////////////////////////////////////////////////////////////

    rgb rgb_transform(const rgb& from, float v, const rgb& to)
    {
        return
        {
            float_transform(from.r, v, to.r),
            float_transform(from.g, v, to.g),
            float_transform(from.b, v, to.b)
        };
    }

    ////////////////////////////////////////////////////////////////////////////////

    std::string to_js_hex_color(const rgbi& c)
    {
        return "0x{:0>2x}{:0>2x}{:0>2x}"_format(c.r, c.g, c.b);
    }

    std::string to_html_rgb(const rgbi& c)
    {
        return "rgb({}, {}, {})"_format(c.r, c.g, c.b);
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
        constexpr std::optional<U8> parse_hex_char(char c)
        {
            switch (c)
            {
            case '0': return {static_cast<U8>(0)};
            case '1': return {static_cast<U8>(1)};
            case '2': return {static_cast<U8>(2)};
            case '3': return {static_cast<U8>(3)};
            case '4': return {static_cast<U8>(4)};
            case '5': return {static_cast<U8>(5)};
            case '6': return {static_cast<U8>(6)};
            case '7': return {static_cast<U8>(7)};
            case '8': return {static_cast<U8>(8)};
            case '9': return {static_cast<U8>(9)};

            case 'a': case 'A': return {static_cast<U8>(10)};
            case 'b': case 'B': return {static_cast<U8>(11)};
            case 'c': case 'C': return {static_cast<U8>(12)};
            case 'd': case 'D': return {static_cast<U8>(13)};
            case 'e': case 'E': return {static_cast<U8>(14)};
            case 'f': case 'F': return {static_cast<U8>(15)};

            default:
                return std::nullopt;
            }
        }

        constexpr std::optional<U8> combine_hex_char(std::optional<U8> pc, std::optional<U8> pd)
        {
            if (pc && pd)
            {
                return static_cast<U8>((*pc<<4) | *pd);
            }
            else
            {
                return std::nullopt;
            }
        }

        // give a #fff or a #ffffff string (depending on size arg), interpret it as a array and parse a index
        template<int size> // is each 1 or 2 chars
        std::pair<std::optional<U8>, std::string_view>
        parse_hex(const std::string_view& value, int index)
        {
            const auto s = value.substr(1 + index * size, size);

            std::optional<U8> r;
            if constexpr (size == 1)
            {
                const auto p = parse_hex_char(s[0]);
                r = combine_hex_char(p, p);
            }
            else if constexpr (size == 2)
            {
                r = combine_hex_char
                (
                    parse_hex_char(s[0]),
                    parse_hex_char(s[1])
                );
            }
            else
            {
                ASSERT(false && "unreachable");
            }

            if (r) { return { *r, s }; }
            else { return { std::nullopt, s }; };
        }

        using R = Result<rgbi>;

        // given a component sie, parses a #fff or a #ffffff string to a color
        template<int size>
        R parse_rgb_hex(const std::string_view& value)
        {
            const auto [r, r_value] = parse_hex<size>(value, 0);
            const auto [g, g_value] = parse_hex<size>(value, 1);
            const auto [b, b_value] = parse_hex<size>(value, 2);
            if (r && g && b)
            {
                return R::create_value({ *r, *g, *b });
            }
            else
            {
                auto invalids = std::vector<std::string>{};
                if (!r) { invalids.emplace_back("red({})"_format(r_value)); }
                if (!g) { invalids.emplace_back("green({})"_format(g_value)); }
                if (!b) { invalids.emplace_back("blue({})"_format(b_value)); }
                return R::create_error
                (
                    "#color contains invalid hex for {}"_format
                    (
                        string_mergers::english_and.merge(invalids)
                    )
                );
            }
        }

        // parses a #fff or a #ffffff string as a color
        R
        parse_hash_hex_rgbi(const std::string& value)
        {
            const auto size = value.length();
            switch(size)
            {
            case 4: return parse_rgb_hex<1>(value);
            case 7: return parse_rgb_hex<2>(value);
            default:
                return R::create_error
                (
                    "a hexadecimal color needs to be either #abc or #aabbcc. "
                    "current count: {}"_format(size-1)
                );
            }
        }
    }

    [[nodiscard]]
    Result<rgbi>
    crgbi(const std::string& original_value)
    {
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
                "bad name. Hex values require a #, but it could also be either {}"_format
                (
                    string_mergers::english_or.merge
                    (
                        match.names
                    )
                )
            );
        }
    }

}
