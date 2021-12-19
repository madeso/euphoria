#pragma once

#include <string>

#include "core/cint.h"
#include "core/colors.h"
#include "core/angle.h"
#include "core/default_parse.h"


namespace euphoria::core
{
    // std::uint8_t colors: 0 - 255
    struct Rgbi;
    struct Rgbai;

    // float based colors: 0.0 - 1.0
    struct Rgb;
    struct Rgba;


    //////////////////////////////////////////////////////////////////////////
    // Rgb no alpha support - int based
    struct Rgbi
    {
        constexpr
        Rgbi(std::uint8_t red, std::uint8_t green, std::uint8_t blue);

        constexpr explicit
        Rgbi(std::uint8_t gray);

        constexpr
        Rgbi(NamedColor color);

        constexpr explicit
        Rgbi(const Rgb& rgb);

        [[nodiscard]] constexpr static Rgbi
        from_hex(unsigned int hex);

        [[nodiscard]] constexpr int
        to_hex() const;

        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
    };


    //////////////////////////////////////////////////////////////////////////
    // Rgb with alpha - int based

    struct Rgbai
    {
        constexpr
        Rgbai(const Rgbi& rgb, std::uint8_t alpha = 255);

        constexpr explicit
        Rgbai(const Rgba& rgba);

        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;
    };


    //////////////////////////////////////////////////////////////////////////
    // Rgb no alpha support - float based

    struct Rgb
    {
        Rgb(float red, float green, float blue);
        explicit Rgb(float gray);

        Rgb(NamedColor color);

        [[nodiscard]]
        static Rgb
        from_hex(unsigned int hex);

        [[nodiscard]] float
        get_length() const;

        [[nodiscard]] float
        get_length_squared() const;

        [[nodiscard]] float
        calc_luminance() const;

        void
        operator+=(const Rgb& rhs);

        void
        operator-=(const Rgb& rhs);

        void
        operator/=(float rhs);

        // memberwise multiplication
        void
        operator*=(const Rgb& rhs);

        float r;
        float g;
        float b;
    };


    Rgb
    operator+(const Rgb& lhs, const Rgb& rhs);

    Rgb
    operator-(const Rgb& lhs, const Rgb& rhs);

    // memberwise multiplication
    Rgb
    operator*(const Rgb& lhs, const Rgb& rhs);

    Rgb
    operator/(const Rgb& lhs, float rhs);

    Rgb
    operator*(const Rgb& lhs, float rhs);

    Rgb
    operator*(float lhs, const Rgb& rhs);

    float
    dot(const Rgb& lhs, const Rgb& rhs);

    Rgb
    clamp(const Rgb& r);


    //////////////////////////////////////////////////////////////////////////
    // HSL

    struct Hsl
    {
        // hue, saturation, lightness
        Angle h; // the most noticeable color that stands out to your eye
        float s; // the "brilliance" (0-1)
        float l; // the light or darkness (0-1)

        // todo(Gustav): why are these angles and not a Hsl?
        static constexpr Angle red = Angle::from_percent_of_360(0.0f);
        static constexpr Angle yellow = Angle::from_percent_of_360(1 / 6.0f);
        static constexpr Angle green = Angle::from_percent_of_360(2 / 6.0f);
        static constexpr Angle cyan = Angle::from_percent_of_360(3 / 6.0f);
        static constexpr Angle blue = Angle::from_percent_of_360(4 / 6.0f);
        static constexpr Angle magenta = Angle::from_percent_of_360(5 / 6.0f);
    };

    enum class IsAbsolute
    {
        yes,
        no
    };

    //////////////////////////////////////////////////////////////////////////
    // Util functions

    Hsl
    saturate(const Hsl& ahsl, float amount, IsAbsolute method);

    Hsl
    desaturate(const Hsl& ahsl, float amount, IsAbsolute method);

    Hsl
    lighten(const Hsl& ahsl, float amount, IsAbsolute method);

    Hsl
    darken(const Hsl& ahsl, float amount, IsAbsolute method);

    /** Makes a color brighter or darker.
     * The +1 makes it white, 0% is no change, -1 makes it black.
     */
    Rgb
    shade_color(const Rgb& color, float percentage);


    //////////////////////////////////////////////////////////////////////////
    // Rgb with alpha - float based

    struct Rgba
    {
    public:
        Rgba(const Rgb& rgb, float alpha = 1.0f);

        float r;
        float g;
        float b;
        float a;
    };


    //////////////////////////////////////////////////////////////////////////
    // For printing

    std::ostream&
    operator<<(std::ostream& stream, const Rgbi& v);

    std::ostream&
    operator<<(std::ostream& stream, const Rgbai& v);

    std::ostream&
    operator<<(std::ostream& stream, const Rgb& v);

    std::ostream&
    operator<<(std::ostream& stream, const Rgba& v);

    std::ostream&
    operator<<(std::ostream& stream, const Hsl& v);

    //////////////////////////////////////////////////////////////////////////
    // Default compare

    bool
    operator==(const Rgbi& lhs, const Rgbi& rhs);

    bool
    operator!=(const Rgbi& lhs, const Rgbi& rhs);

    bool
    operator==(const Rgbai& lhs, const Rgbai& rhs);

    bool
    operator!=(const Rgbai& lhs, const Rgbai& rhs);

    //////////////////////////////////////////////////////////////////////////
    // Convert functions (rgb)

    Rgb
    crgb(const Rgbi& rgb);

    Rgb
    crgb(const Rgba& rgb);

    Rgb
    crgb(const Rgbai& rgb);

    Rgb
    crgb(const Hsl& c);

    // Convert functions (hsl)

    Hsl
    chsl(const Rgb& c);

    // Convert functions (rgbi)

    Rgbi
    crgbi(const Rgb& rgb);

    Rgbi
    crgbi(const Rgba& rgb);

    Rgbi
    crgbi(const Rgbai& rgb);

    // convert functions rgba
    Rgba
    crgba(const Rgbai& rgb);

    Rgbai
    crgbai(const Rgba& rgb);


    //////////////////////////////////////////////////////////////////////////
    // Transforms

    struct RgbTransform
    {
        static Rgb
        transform(const Rgb& from, float v, const Rgb& to);
    };


    //////////////////////////////////////////////////////////////////////////
    // Parsing
    [[nodiscard]]
    Result<Rgbi>
    crgbi(const std::string& value);

    template<>
    struct CustomArgparser<Rgbi>
    {
        enum { value = 1 };

        static
        std::string
        to_string(const Rgbi& c)
        {
            std::ostringstream ss;
            ss << c;
            return ss.str();
        }

        static
        Result<Rgbi>
        parse(const std::string& value)
        {
            return crgbi(value);
        }
    };


    //////////////////////////////////////////////////////////////////////////
    // Internal utility

    namespace colorutil
    {
        // internal function, exposed for unit tests
        constexpr
        U8
        get_component(unsigned int i, unsigned int steps)
        {
            const auto bit_steps = 8 * steps;
            const auto value = static_cast<int>((i >> bit_steps) & 0xff);
            return static_cast<U8>(value);
        }

        constexpr
        U8
        get_red(unsigned int rgb)
        {
            return get_component(rgb, 2);
        }

        constexpr
        U8
        get_green(unsigned int rgb)
        {
            return get_component(rgb, 1);
        }

        constexpr
        U8
        get_blue(unsigned int rgb)
        {
            return get_component(rgb, 0);
        }

        constexpr
        float
        to_float(std::uint8_t c)
        {
            return static_cast<float>(c) / 255.0f;
        }

        constexpr
        U8
        to_unsigned_char(float f)
        {
            return static_cast<U8>(static_cast<int>(f * 255.0f));
        }

        constexpr
        unsigned int
        to_color_hex(unsigned int r, unsigned int g, unsigned int b)
        {
            return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
        }

        constexpr
        unsigned int
        to_color_hex(NamedColor color)
        {
            switch(color)
            {
            case NamedColor::white: return to_color_hex(255, 255, 255);
            case NamedColor::light_gray: return to_color_hex(160, 160, 160);
            case NamedColor::gray: return to_color_hex(127, 127, 127);
            case NamedColor::dark_gray: return to_color_hex(87, 87, 87);
            case NamedColor::black: return to_color_hex(0, 0, 0);
            case NamedColor::red: return to_color_hex(173, 35, 35);
            case NamedColor::pure_red: return to_color_hex(255, 0, 0);
            case NamedColor::blue: return to_color_hex(42, 75, 215);
            case NamedColor::pure_blue: return to_color_hex(0, 0, 255);
            case NamedColor::light_blue: return to_color_hex(157, 175, 255);
            case NamedColor::normal_blue: return to_color_hex(127, 127, 255);
            case NamedColor::cornflower_blue: return to_color_hex(100, 149, 237);
            case NamedColor::green: return to_color_hex(29, 105, 20);
            case NamedColor::pure_green: return to_color_hex(0, 255, 0);
            case NamedColor::light_green: return to_color_hex(129, 197, 122);
            case NamedColor::yellow: return to_color_hex(255, 238, 51);
            case NamedColor::pure_yellow: return to_color_hex(255, 255, 0);
            case NamedColor::orange: return to_color_hex(255, 146, 51);
            case NamedColor::pure_orange: return to_color_hex(255, 127, 0);
            case NamedColor::brown: return to_color_hex(129, 74, 25);
            case NamedColor::pure_brown: return to_color_hex(250, 75, 0);
            case NamedColor::purple: return to_color_hex(129, 38, 192);
            case NamedColor::pure_purple: return to_color_hex(128, 0, 128);
            case NamedColor::pink: return to_color_hex(255, 205, 243);
            case NamedColor::pure_pink: return to_color_hex(255, 192, 203);
            case NamedColor::pure_beige: return to_color_hex(245, 245, 220);
            case NamedColor::tan: return to_color_hex(233, 222, 187);
            case NamedColor::pure_tan: return to_color_hex(210, 180, 140);
            case NamedColor::cyan: return to_color_hex(41, 208, 208);
            case NamedColor::pure_cyan: return to_color_hex(0, 255, 255);

            default: return to_color_hex(0, 0, 0);
            }
        }

        unsigned int
        from_string_to_hex(const std::string& str);
    }



    // -----------------------------------------------------------------------
    // implementation

    constexpr
    Rgbi::Rgbi(std::uint8_t red, std::uint8_t green, std::uint8_t blue)
        : r(red), g(green), b(blue)
    {
    }


    constexpr
    Rgbi::Rgbi(std::uint8_t gray)
        : r(gray), g(gray), b(gray)
    {
    }


    constexpr
    Rgbi::Rgbi(NamedColor color)
        : Rgbi(Rgbi::from_hex(colorutil::to_color_hex(color)))
    {
    }


    constexpr
    Rgbi::Rgbi(const Rgb& rgb)
        : r(colorutil::to_unsigned_char(rgb.r))
        , g(colorutil::to_unsigned_char(rgb.g))
        , b(colorutil::to_unsigned_char(rgb.b))
    {}


    [[nodiscard]] constexpr Rgbi
    Rgbi::from_hex(unsigned int hex)
    {
        return
        {
            colorutil::get_red(hex),
            colorutil::get_green(hex),
            colorutil::get_blue(hex)
        };
    }


    constexpr
    int
    Rgbi::to_hex() const
    {
        const auto value = [](int i, int steps) -> int { return i << (8 * steps); };
        return value(r, 2) | value(g, 1) | value(b, 0);
    }


    constexpr
    Rgbai::Rgbai(const Rgbi& rgb, std::uint8_t alpha)
        : r(rgb.r), g(rgb.g), b(rgb.b), a(alpha)
    {
    }


    constexpr
    Rgbai::Rgbai(const Rgba& rgba)
        : r(colorutil::to_unsigned_char(rgba.r))
        , g(colorutil::to_unsigned_char(rgba.g))
        , b(colorutil::to_unsigned_char(rgba.b))
        , a(colorutil::to_unsigned_char(rgba.a))
    {
    }

}
