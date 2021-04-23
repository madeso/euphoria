#ifndef CORE_RGB_H
#define CORE_RGB_H

#include <string>
#include <cstdint>

#include "core/colors.h"
#include "core/angle.h"
#include "core/default_parse.h"


namespace euphoria::core
{
    // std::uint8_t colors: 0 - 255
    struct rgbi;
    struct rgbai;

    // float based colors: 0.0 - 1.0
    struct rgb;
    struct rgba;


    //////////////////////////////////////////////////////////////////////////
    // Rgb no alpha support - int based
    struct rgbi
    {
        constexpr
        rgbi(std::uint8_t red, std::uint8_t green, std::uint8_t blue);

        constexpr explicit
        rgbi(std::uint8_t gray);

        constexpr
        rgbi(color color);

        constexpr explicit
        rgbi(const rgb& rgb);

        [[nodiscard]] constexpr static rgbi
        from_hex(unsigned int hex);

        [[nodiscard]] constexpr int
        to_hex() const;

        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
    };


    //////////////////////////////////////////////////////////////////////////
    // Rgb with alpha - int based

    struct rgbai
    {
        constexpr
        rgbai(const rgbi& rgb, std::uint8_t alpha = 255);

        constexpr explicit
        rgbai(const rgba& rgba);

        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;
    };


    //////////////////////////////////////////////////////////////////////////
    // Rgb no alpha support - float based

    struct rgb
    {
        rgb(float red, float green, float blue);
        explicit rgb(float gray);

        rgb(color color);

        [[nodiscard]]
        static rgb
        from_hex(unsigned int hex);

        [[nodiscard]] float
        get_length() const;

        [[nodiscard]] float
        get_length_squared() const;

        [[nodiscard]] float
        calc_luminance() const;

        void
        operator+=(const rgb& rhs);

        void
        operator-=(const rgb& rhs);

        void
        operator/=(float rhs);

        // memberwise multiplication
        void
        operator*=(const rgb& rhs);

        float r;
        float g;
        float b;
    };


    rgb
    operator+(const rgb& lhs, const rgb& rhs);

    rgb
    operator-(const rgb& lhs, const rgb& rhs);

    // memberwise multiplication
    rgb
    operator*(const rgb& lhs, const rgb& rhs);

    rgb
    operator/(const rgb& lhs, float rhs);

    rgb
    operator*(const rgb& lhs, float rhs);

    rgb
    operator*(float lhs, const rgb& rhs);

    float
    dot(const rgb& lhs, const rgb& rhs);

    rgb
    clamp(const rgb& r);


    //////////////////////////////////////////////////////////////////////////
    // HSL

    struct hsl
    {
        // hue, saturation, lightness
        angle h;
        float s;  // 0-1
        float l;  // 0-1

        // todo(Gustav): why are these angles and not a Hsl?
        static constexpr angle red = angle::from_percent_of_360(0.0f);
        static constexpr angle yellow = angle::from_percent_of_360(1 / 6.0f);
        static constexpr angle green = angle::from_percent_of_360(2 / 6.0f);
        static constexpr angle cyan = angle::from_percent_of_360(3 / 6.0f);
        static constexpr angle blue = angle::from_percent_of_360(4 / 6.0f);
        static constexpr angle magenta = angle::from_percent_of_360(5 / 6.0f);
    };

    enum class is_absolute
    {
        yes,
        no
    };

    //////////////////////////////////////////////////////////////////////////
    // Util functions

    hsl
    saturate(const hsl& ahsl, float amount, is_absolute method);

    hsl
    desaturate(const hsl& ahsl, float amount, is_absolute method);

    hsl
    lighten(const hsl& ahsl, float amount, is_absolute method);

    hsl
    darken(const hsl& ahsl, float amount, is_absolute method);

    /** Makes a color brighter or darker.
     * The +1 makes it white, 0% is no change, -1 makes it black.
     */
    rgb
    shade_color(const rgb& color, float percentage);


    //////////////////////////////////////////////////////////////////////////
    // Rgb with alpha - float based

    struct rgba
    {
    public:
        rgba(const rgb& rgb, float alpha = 1.0f);

        float r;
        float g;
        float b;
        float a;
    };


    //////////////////////////////////////////////////////////////////////////
    // For printing

    std::ostream&
    operator<<(std::ostream& stream, const rgbi& v);

    std::ostream&
    operator<<(std::ostream& stream, const rgbai& v);

    std::ostream&
    operator<<(std::ostream& stream, const rgb& v);

    std::ostream&
    operator<<(std::ostream& stream, const rgba& v);

    std::ostream&
    operator<<(std::ostream& stream, const hsl& v);

    //////////////////////////////////////////////////////////////////////////
    // Default compare

    bool
    operator==(const rgbi& lhs, const rgbi& rhs);

    bool
    operator!=(const rgbi& lhs, const rgbi& rhs);

    bool
    operator==(const rgbai& lhs, const rgbai& rhs);

    bool
    operator!=(const rgbai& lhs, const rgbai& rhs);

    //////////////////////////////////////////////////////////////////////////
    // Convert functions (rgb)

    rgb
    crgb(const rgbi& rgb);

    rgb
    crgb(const rgba& rgb);

    rgb
    crgb(const rgbai& rgb);

    rgb
    crgb(const hsl& c);

    // Convert functions (hsl)

    hsl
    chsl(const rgb& c);

    // Convert functions (rgbi)

    rgbi
    crgbi(const rgb& rgb);

    rgbi
    crgbi(const rgba& rgb);

    rgbi
    crgbi(const rgbai& rgb);

    // convert functions rgba
    rgba
    crgba(const rgbai& rgb);

    rgbai
    crgbai(const rgba& rgb);


    //////////////////////////////////////////////////////////////////////////
    // Transforms

    struct rgb_transform
    {
        static rgb
        Transform(const rgb& from, float v, const rgb& to);
    };


    //////////////////////////////////////////////////////////////////////////
    // Parsing
    [[nodiscard]]
    Result<rgbi>
    crgbi(const std::string& value);

    template<>
    struct custom_argparser<rgbi>
    {
        enum { value = 1 };

        static
        std::string
        to_string(const rgbi& c)
        {
            std::ostringstream ss;
            ss << c;
            return ss.str();
        }

        static
        Result<rgbi>
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
        std::uint8_t
        get_component(unsigned int i, unsigned int steps)
        {
            const auto bit_steps = 8 * steps;
            const auto value = ((i >> bit_steps) & 0xff);
            return value;
        }

        constexpr
        std::uint8_t
        get_red(unsigned int rgb)
        {
            return get_component(rgb, 2);
        }

        constexpr
        std::uint8_t
        get_green(unsigned int rgb)
        {
            return get_component(rgb, 1);
        }

        constexpr
        std::uint8_t
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
        std::uint8_t
        to_unsigned_char(float f)
        {
            return static_cast<std::uint8_t>(f * 255.0f);
        }

        constexpr
        unsigned int
        to_color_hex(unsigned int r, unsigned int g, unsigned int b)
        {
            return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
        }

        constexpr
        unsigned int
        to_color_hex(color color)
        {
            switch(color)
            {
            case color::white: return to_color_hex(255, 255, 255);
            case color::light_gray: return to_color_hex(160, 160, 160);
            case color::gray: return to_color_hex(127, 127, 127);
            case color::dark_gray: return to_color_hex(87, 87, 87);
            case color::black: return to_color_hex(0, 0, 0);
            case color::red: return to_color_hex(173, 35, 35);
            case color::pure_red: return to_color_hex(255, 0, 0);
            case color::blue: return to_color_hex(42, 75, 215);
            case color::pure_blue: return to_color_hex(0, 0, 255);
            case color::light_blue: return to_color_hex(157, 175, 255);
            case color::normal_blue: return to_color_hex(127, 127, 255);
            case color::cornflower_blue: return to_color_hex(100, 149, 237);
            case color::green: return to_color_hex(29, 105, 20);
            case color::pure_green: return to_color_hex(0, 255, 0);
            case color::light_green: return to_color_hex(129, 197, 122);
            case color::yellow: return to_color_hex(255, 238, 51);
            case color::pure_yellow: return to_color_hex(255, 255, 0);
            case color::orange: return to_color_hex(255, 146, 51);
            case color::pure_orange: return to_color_hex(255, 127, 0);
            case color::brown: return to_color_hex(129, 74, 25);
            case color::pure_brown: return to_color_hex(250, 75, 0);
            case color::purple: return to_color_hex(129, 38, 192);
            case color::pure_purple: return to_color_hex(128, 0, 128);
            case color::pink: return to_color_hex(255, 205, 243);
            case color::pure_pink: return to_color_hex(255, 192, 203);
            case color::pure_beige: return to_color_hex(245, 245, 220);
            case color::tan: return to_color_hex(233, 222, 187);
            case color::pure_tan: return to_color_hex(210, 180, 140);
            case color::cyan: return to_color_hex(41, 208, 208);
            case color::pure_cyan: return to_color_hex(0, 255, 255);

            default: return to_color_hex(0, 0, 0);
            }
        }

        unsigned int
        from_string_to_hex(const std::string& str);
    }  // namespace colorutil



    // -----------------------------------------------------------------------
    // implementation

    constexpr
    rgbi::rgbi(std::uint8_t red, std::uint8_t green, std::uint8_t blue)
        : r(red), g(green), b(blue)
    {
    }


    constexpr
    rgbi::rgbi(std::uint8_t gray)
        : r(gray), g(gray), b(gray)
    {
    }


    constexpr
    rgbi::rgbi(color color)
        : rgbi(rgbi::from_hex(colorutil::to_color_hex(color)))
    {
    }


    constexpr
    rgbi::rgbi(const rgb& rgb)
        : r(colorutil::to_unsigned_char(rgb.r))
        , g(colorutil::to_unsigned_char(rgb.g))
        , b(colorutil::to_unsigned_char(rgb.b))
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
    rgbai::rgbai(const rgbi& rgb, std::uint8_t alpha)
        : r(rgb.r), g(rgb.g), b(rgb.b), a(alpha)
    {
    }


    constexpr
    rgbai::rgbai(const rgba& rgba)
        : r(colorutil::to_unsigned_char(rgba.r))
        , g(colorutil::to_unsigned_char(rgba.g))
        , b(colorutil::to_unsigned_char(rgba.b))
        , a(colorutil::to_unsigned_char(rgba.a))
    {
    }

}  // namespace euphoria::core

#endif  // CORE_RGB_H
