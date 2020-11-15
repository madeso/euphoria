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
        Rgbi(Color color);

        constexpr explicit
        Rgbi(const Rgb& rgb);

        [[nodiscard]] constexpr static Rgbi
        FromHex(unsigned int hex);

        constexpr int
        ToHex() const;

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

        Rgb(Color color);

        [[nodiscard]]
        static Rgb
        FromHex(unsigned int hex);

        float
        GetLength() const;

        float
        GetLengthSquared() const;

        float
        CalcLuminance() const;

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
    Clamp(const Rgb& r);


    //////////////////////////////////////////////////////////////////////////
    // HSL

    struct Hsl
    {
        // hue, saturation, lightness
        Angle h;
        float s;  // 0-1
        float l;  // 0-1

        // todo(Gustav): why do theese functions return a angle and not a Hsl?

        [[nodiscard]] static const Angle&
        Red();

        [[nodiscard]] static const Angle&
        Yellow();

        [[nodiscard]] static const Angle&
        Green();

        [[nodiscard]] static const Angle&
        Cyan();

        [[nodiscard]] static const Angle&
        Blue();

        [[nodiscard]] static const Angle&
        Magenta();
    };

    enum class Method
    {
        Absolute,
        Relative
    };

    //////////////////////////////////////////////////////////////////////////
    // Util functions

    Hsl
    saturate(const Hsl& ahsl, float amount, Method method);

    Hsl
    desaturate(const Hsl& ahsl, float amount, Method method);

    Hsl
    lighten(const Hsl& ahsl, float amount, Method method);

    Hsl
    darken(const Hsl& ahsl, float amount, Method method);

    /** Makes a color brighter or darker.
     * The +1 makes it white, 0% is no change, -1 makes it black.
     */
    Rgb
    ShadeColor(const Rgb& rgb, float percentage);


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
    rgb(const Rgbi& rgb);

    Rgb
    rgb(const Rgba& rgb);

    Rgb
    rgb(const Rgbai& rgb);

    Rgb
    rgb(const Hsl& c);

    // Convert functions (hsl)

    Hsl
    hsl(const Rgb& c);

    // Convert functions (rgbi)

    Rgbi
    rgbi(const Rgb& rgb);

    Rgbi
    rgbi(const Rgba& rgb);

    Rgbi
    rgbi(const Rgbai& rgb);

    // convert functions rgba
    Rgba
    rgba(const Rgbai& rgb);

    Rgbai
    rgbai(const Rgba& rgb);


    //////////////////////////////////////////////////////////////////////////
    // Transforms

    struct RgbTransform
    {
        static Rgb
        Transform(const Rgb& from, float v, const Rgb& to);
    };


    //////////////////////////////////////////////////////////////////////////
    // Parsing
    [[nodiscard]]
    Result<Rgbi>
    rgbi(const std::string& value);

    template<>
    struct CustomArgparser<Rgbi>
    {
        enum { value = 1 };

        static
        std::string
        ToString(const Rgbi& c)
        {
            std::ostringstream ss;
            ss << c;
            return ss.str();
        }

        static
        Result<Rgbi>
        Parse(const std::string& value)
        {
            return rgbi(value);
        }
    };


    //////////////////////////////////////////////////////////////////////////
    // Internal utility

    namespace colorutil
    {
        // internal function, exposed for unit tests
        constexpr
        std::uint8_t
        GetComponent(unsigned int i, int steps)
        {
            const int value = ((i >> 8 * steps) & 0xff);
            return static_cast<std::uint8_t>(value);
        }

        constexpr
        std::uint8_t
        GetRed(unsigned int rgb)
        {
            return GetComponent(rgb, 2);
        }

        constexpr
        std::uint8_t
        GetGreen(unsigned int rgb)
        {
            return GetComponent(rgb, 1);
        }

        constexpr
        std::uint8_t
        GetBlue(unsigned int rgb)
        {
            return GetComponent(rgb, 0);
        }

        constexpr
        float
        ToFloat(std::uint8_t c)
        {
            return c / 255.0f;
        }

        constexpr
        std::uint8_t
        ToUnsignedChar(float f)
        {
            return static_cast<std::uint8_t>(f * 255.0f);
        }

        constexpr
        unsigned int
        RGB(unsigned int r, unsigned int g, unsigned int b)
        {
            return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
        }

        constexpr
        unsigned int
        ToColorHex(Color color)
        {
            switch(color)
            {
            case Color::White: return RGB(255, 255, 255);
            case Color::LightGray: return RGB(160, 160, 160);
            case Color::Gray: return RGB(127, 127, 127);
            case Color::DarkGray: return RGB(87, 87, 87);
            case Color::Black: return RGB(0, 0, 0);
            case Color::Red: return RGB(173, 35, 35);
            case Color::PureRed: return RGB(255, 0, 0);
            case Color::Blue: return RGB(42, 75, 215);
            case Color::PureBlue: return RGB(0, 0, 255);
            case Color::LightBlue: return RGB(157, 175, 255);
            case Color::NormalBlue: return RGB(127, 127, 255);
            case Color::CornflowerBlue: return RGB(100, 149, 237);
            case Color::Green: return RGB(29, 105, 20);
            case Color::PureGreen: return RGB(0, 255, 0);
            case Color::LightGreen: return RGB(129, 197, 122);
            case Color::Yellow: return RGB(255, 238, 51);
            case Color::PureYellow: return RGB(255, 255, 0);
            case Color::Orange: return RGB(255, 146, 51);
            case Color::PureOrange: return RGB(255, 127, 0);
            case Color::Brown: return RGB(129, 74, 25);
            case Color::PureBrown: return RGB(250, 75, 0);
            case Color::Purple: return RGB(129, 38, 192);
            case Color::PurePurple: return RGB(128, 0, 128);
            case Color::Pink: return RGB(255, 205, 243);
            case Color::PurePink: return RGB(255, 192, 203);
            case Color::PureBeige: return RGB(245, 245, 220);
            case Color::Tan: return RGB(233, 222, 187);
            case Color::PureTan: return RGB(210, 180, 140);
            case Color::Cyan: return RGB(41, 208, 208);
            case Color::PureCyan: return RGB(0, 255, 255);

            default: return RGB(0, 0, 0);
            }
        }

        unsigned int
        FromStringToHex(const std::string& str);
    }  // namespace colorutil



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
    Rgbi::Rgbi(Color color)
        : Rgbi(Rgbi::FromHex(colorutil::ToColorHex(color)))
    {
    }


    constexpr
    Rgbi::Rgbi(const Rgb& rgb)
        : r(colorutil::ToUnsignedChar(rgb.r))
        , g(colorutil::ToUnsignedChar(rgb.g))
        , b(colorutil::ToUnsignedChar(rgb.b))
    {}


    [[nodiscard]] constexpr Rgbi
    Rgbi::FromHex(unsigned int hex)
    {
        return
        {
            colorutil::GetRed(hex),
            colorutil::GetGreen(hex),
            colorutil::GetBlue(hex)
        };
    }


    constexpr
    int
    Rgbi::ToHex() const
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
        : r(colorutil::ToUnsignedChar(rgba.r))
        , g(colorutil::ToUnsignedChar(rgba.g))
        , b(colorutil::ToUnsignedChar(rgba.b))
        , a(colorutil::ToUnsignedChar(rgba.a))
    {
    }

}  // namespace euphoria::core

#endif  // CORE_RGB_H
