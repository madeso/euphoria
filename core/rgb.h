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
        Rgbi(std::uint8_t red, std::uint8_t green, std::uint8_t blue);
        explicit Rgbi(std::uint8_t gray);
        Rgbi(Color color);

        explicit Rgbi(const Rgb& rgb);

        [[nodiscard]] static Rgbi
        FromHex(unsigned int hex);

        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
    };

    //////////////////////////////////////////////////////////////////////////
    // Rgb with alpha - int based

    struct Rgbai
    {
        Rgbai(const Rgbi& rgb, std::uint8_t alpha = 255);

        explicit Rgbai(const Rgba& rgba);

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

        void
        operator+=(const Rgb& rhs);

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
    operator==(const Rgbai& lhs, const Rgbai& rhs);

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
        std::uint8_t
        GetComponent(unsigned int i, int steps);

        std::uint8_t
        GetRed(unsigned int rgb);

        std::uint8_t
        GetGreen(unsigned int rgb);

        std::uint8_t
        GetBlue(unsigned int rgb);

        float
        ToFloat(std::uint8_t c);

        std::uint8_t
        ToUnsignedChar(float f);

        unsigned int
        FromStringToHex(const std::string& str);
    }  // namespace colorutil

}  // namespace euphoria::core

#endif  // CORE_RGB_H
