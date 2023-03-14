#pragma once



#include "core/cint.h"
#include "core/colors.h"
#include "core/angle.h"
#include "core/interpolate.default.h"
#include "core/default_parse.h"


namespace euphoria::core
{
    // U8 colors: 0 - 255
    struct rgbi;
    struct rgbai;

    // float based colors: 0.0 - 1.0
    struct rgb;
    struct rgba;


    //////////////////////////////////////////////////////////////////////////
    // rgb no alpha support - int based
    struct rgbi
    {
        U8 r;
        U8 g;
        U8 b;

        constexpr rgbi(U8 red, U8 green, U8 blue);
        constexpr explicit rgbi(U8 gray);
        constexpr rgbi(NamedColor color);
        constexpr explicit rgbi(const rgb& c);

        [[nodiscard]] constexpr static rgbi from_hex(unsigned int hex);
        [[nodiscard]] constexpr int to_hex() const;
    };


    //////////////////////////////////////////////////////////////////////////
    // rgb with alpha - int based

    struct rgbai
    {
        U8 r;
        U8 g;
        U8 b;
        U8 a;

        constexpr rgbai(const rgbi& c, U8 alpha = 255);
        constexpr rgbai(U8 red, U8 green, U8 blue, U8 alpha = 255);
        constexpr rgbai(U8 gray, U8 alpha = 255);
        constexpr explicit rgbai(const rgba& c);
    };


    //////////////////////////////////////////////////////////////////////////
    // rgb no alpha support - float based

    struct rgb
    {
        float r;
        float g;
        float b;

        rgb(float red, float green, float blue);
        explicit rgb(float gray);

        rgb(NamedColor color);

        [[nodiscard]] static rgb from_hex(unsigned int hex);
        [[nodiscard]] float get_length() const;
        [[nodiscard]] float get_length_squared() const;
        [[nodiscard]] float calc_luminance() const;

        void operator+=(const rgb& rhs);
        void operator-=(const rgb& rhs);
        void operator/=(float rhs);

        // memberwise multiplication
        void operator*=(const rgb& rhs);

        
    };


    rgb operator+(const rgb& lhs, const rgb& rhs);
    rgb operator-(const rgb& lhs, const rgb& rhs);

    // memberwise multiplication
    rgb operator*(const rgb& lhs, const rgb& rhs);
    rgb operator/(const rgb& lhs, float rhs);
    rgb operator*(const rgb& lhs, float rhs);
    rgb operator*(float lhs, const rgb& rhs);
    float dot(const rgb& lhs, const rgb& rhs);
    rgb clamp(const rgb& r);


    //////////////////////////////////////////////////////////////////////////
    // HSL

    struct Hsl
    {
        // hue, saturation, lightness
        angle h; // the most noticeable color that stands out to your eye
        float s; // the "brilliance" (0-1)
        float l; // the light or darkness (0-1)

        // todo(Gustav): why are these angles and not a Hsl?
        static constexpr angle red = angle::from_percent_of_360(0.0f);
        static constexpr angle yellow = angle::from_percent_of_360(1 / 6.0f);
        static constexpr angle green = angle::from_percent_of_360(2 / 6.0f);
        static constexpr angle cyan = angle::from_percent_of_360(3 / 6.0f);
        static constexpr angle blue = angle::from_percent_of_360(4 / 6.0f);
        static constexpr angle magenta = angle::from_percent_of_360(5 / 6.0f);
    };

    enum class IsAbsolute
    {
        yes,
        no
    };

    //////////////////////////////////////////////////////////////////////////
    // Util functions

    Hsl saturate(const Hsl& ahsl, float amount, IsAbsolute method);
    Hsl desaturate(const Hsl& ahsl, float amount, IsAbsolute method);
    Hsl lighten(const Hsl& ahsl, float amount, IsAbsolute method);
    Hsl darken(const Hsl& ahsl, float amount, IsAbsolute method);

    /** Makes a color brighter or darker.
     * The +1 makes it white, 0% is no change, -1 makes it black.
     */
    rgb shade_color(const rgb& color, float percentage);


    //////////////////////////////////////////////////////////////////////////
    // rgb with alpha - float based

    struct rgba
    {
        float r;
        float g;
        float b;
        float a;

        rgba(float red, float green, float blue, float alpha = 1.0f);
        rgba(const rgb& c, float alpha = 1.0f);
    };


    //////////////////////////////////////////////////////////////////////////
    // For printing

    std::string to_string(const rgbi& v);
    std::string to_string(const rgbai& v);
    std::string to_string(const rgb& v);
    std::string to_string(const rgba& v);
    std::string to_string(const Hsl& v);

    std::ostream& operator<<(std::ostream& stream, const rgbi& v);
    std::ostream& operator<<(std::ostream& stream, const rgbai& v);
    std::ostream& operator<<(std::ostream& stream, const rgb& v);
    std::ostream& operator<<(std::ostream& stream, const rgba& v);
    std::ostream& operator<<(std::ostream& stream, const Hsl& v);

    //////////////////////////////////////////////////////////////////////////
    // Default compare

    bool operator==(const rgbi& lhs, const rgbi& rhs);
    bool operator!=(const rgbi& lhs, const rgbi& rhs);

    bool operator==(const rgbai& lhs, const rgbai& rhs);
    bool operator!=(const rgbai& lhs, const rgbai& rhs);

    //////////////////////////////////////////////////////////////////////////
    // Convert functions (rgb)

    rgb crgb(const rgbi& c);
    rgb crgb(const rgba& c);
    rgb crgb(const rgbai& c);
    rgb crgb(const Hsl& c);

    // Convert functions (hsl)

    Hsl chsl(const rgb& c);

    // Convert functions (rgbi)

    rgbi crgbi(const rgb& c);
    rgbi crgbi(const rgba& c);
    rgbi crgbi(const rgbai& c);

    // convert functions rgba
    rgba crgba(const rgbai& c);
    rgbai crgbai(const rgba& c);


    //////////////////////////////////////////////////////////////////////////
    // Transforms

    rgb rgb_transform(const rgb& from, float v, const rgb& to);

    DEFAULT_INTERPOLATE(rgb, rgb_transform);


    //////////////////////////////////////////////////////////////////////////
    // Printing/Exporting
    std::string to_js_hex_color(const rgbi& c);
    std::string to_html_rgb(const rgbi& c);


    //////////////////////////////////////////////////////////////////////////
    // Parsing
    [[nodiscard]] Result<rgbi> crgbi(const std::string& value);

    template<>
    struct CustomArgparser<rgbi>
    {
        enum { value = 1 };

        static std::string to_string(const rgbi& c)
        {
            return euphoria::core::to_string(c);
        }

        static Result<rgbi> parse(const std::string& value)
        {
            return crgbi(value);
        }
    };


    //////////////////////////////////////////////////////////////////////////
    // Internal utility

    namespace colorutil
    {
        // internal function, exposed for unit tests
        constexpr U8 get_component(unsigned int i, unsigned int steps)
        {
            const auto bit_steps = 8 * steps;
            const auto value = static_cast<int>((i >> bit_steps) & 0xff);
            return static_cast<U8>(value);
        }

        constexpr U8 get_red(unsigned int c)
        {
            return get_component(c, 2);
        }

        constexpr U8 get_green(unsigned int c)
        {
            return get_component(c, 1);
        }

        constexpr U8 get_blue(unsigned int c)
        {
            return get_component(c, 0);
        }

        constexpr float to_float(U8 c)
        {
            return static_cast<float>(c) / 255.0f;
        }

        constexpr U8 to_unsigned_char(float f)
        {
            return static_cast<U8>(static_cast<int>(f * 255.0f));
        }

        constexpr unsigned int to_color_hex
            (unsigned int r, unsigned int g, unsigned int b)
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


        constexpr
        unsigned int
        to_color_hex(BangWongPalette color)
        {
            switch(color)
            {
            case BangWongPalette::black: return to_color_hex(0, 0, 0);
            case BangWongPalette::organge: return to_color_hex(230, 159, 0);
            case BangWongPalette::blue_sky: return to_color_hex(86, 180, 233);
            case BangWongPalette::green_bluish: return to_color_hex(0, 158, 115);
            case BangWongPalette::yellow: return to_color_hex(240, 228, 66);
            case BangWongPalette::blue: return to_color_hex(0, 114, 178);
            case BangWongPalette::red_vermillion: return to_color_hex(213, 94, 0);
            case BangWongPalette::purple_redish: return to_color_hex(204, 121, 167);
            default: return to_color_hex(0, 0, 0);
            }
        }

        unsigned int
        from_string_to_hex(const std::string& str);
    }



    // -----------------------------------------------------------------------
    // implementation

    constexpr
    rgbi::rgbi(U8 red, U8 green, U8 blue)
        : r(red), g(green), b(blue)
    {
    }


    constexpr
    rgbi::rgbi(U8 gray)
        : r(gray), g(gray), b(gray)
    {
    }


    constexpr
    rgbi::rgbi(NamedColor color)
        : rgbi(rgbi::from_hex(colorutil::to_color_hex(color)))
    {
    }


    constexpr
    rgbi::rgbi(const rgb& c)
        : r(colorutil::to_unsigned_char(c.r))
        , g(colorutil::to_unsigned_char(c.g))
        , b(colorutil::to_unsigned_char(c.b))
    {}


    [[nodiscard]] constexpr rgbi
    rgbi::from_hex(unsigned int hex)
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
    rgbi::to_hex() const
    {
        const auto value = [](int i, int steps) -> int { return i << (8 * steps); };
        return value(r, 2) | value(g, 1) | value(b, 0);
    }


    constexpr
    rgbai::rgbai(const rgbi& c, U8 alpha)
        : r(c.r), g(c.g), b(c.b), a(alpha)
    {
    }


    constexpr
    rgbai::rgbai(U8 red, U8 green, U8 blue, U8 alpha)
        : r(red), g(green), b(blue), a(alpha)
    {
    }

    constexpr
    rgbai::rgbai(U8 gray, U8 alpha)
        : r(gray), g(gray), b(gray), a(alpha)
    {
    }


    constexpr
    rgbai::rgbai(const rgba& c)
        : r(colorutil::to_unsigned_char(c.r))
        , g(colorutil::to_unsigned_char(c.g))
        , b(colorutil::to_unsigned_char(c.b))
        , a(colorutil::to_unsigned_char(c.a))
    {
    }

}
