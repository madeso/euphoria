#include "core/imagefilters.h"

#include "core/image.h"
#include "core/palette.h"
#include "core/table.h"
#include "core/vec3.h"

#include <cmath>

namespace euphoria::core
{
    namespace
    {
        rgbai
        Gray(unsigned char g, unsigned char a)
        {
            return {rgbi {g, g, g}, a};
        }
    }  // namespace


    //
    // https://twitter.com/FreyaHolmer/status/1116502994684530688
    rgbai
    MakeGrayscale(rgbai c, Grayscale grayscale)
    {
        switch(grayscale)
        {
        case Grayscale::R: return Gray(c.r, c.a);
        case Grayscale::G: return Gray(c.g, c.a);
        case Grayscale::B: return Gray(c.b, c.a);
        case Grayscale::A: return Gray(c.a, c.a);
        case Grayscale::Max: return Gray(Max(c.r, Max(c.g, c.b)), c.a);
        case Grayscale::Gamma:
            {
                const auto d = dot(crgb(c), rgb(0.22f, 0.707f, 0.071f));
                return rgbai(crgbi(rgb(d)), c.a);
            }
        case Grayscale::Linear:
            {
                const auto d = dot(crgb(c), rgb(0.0397f, 0.4580f, 0.0061f));
                return rgbai(crgbi(rgb(d)), c.a);
            }
        case Grayscale::Average:
            {
                const auto cc = crgb(c);
                const auto g  = (cc.r + cc.g + cc.b) / 3;
                return rgbai(crgbi(rgb(g)), c.a);
            }

        default:
            return Gray(c.a, c.a);
        }
    }


    void
    MakeGrayscale(Image* image, Grayscale grayscale)
    {
        image->Filter([grayscale](const rgbai& c)
        {
            return MakeGrayscale(c, grayscale);
        });
    }

    void
    MatchPalette(Image* image, const Palette& palette)
    {
        image->Filter([&palette](const rgbai& c) {
            const auto cc = crgbi(c);
            const auto nc = palette.GetClosestColor(cc);

            return rgbai(nc, c.a);
        });
    }

    template <typename C>
    [[nodiscard]] Image
    NewImageFrom(const Image& image, C callback)
    {
        Image ret;
        if(image.HasAlpha())
        {
            ret.SetupWithAlphaSupport(image.GetWidth(), image.GetHeight(), -1);
        }
        else
        {
            ret.SetupNoAlphaSupport(image.GetWidth(), image.GetHeight(), -1);
        }
        ret.SetAllTopBottom(callback);
        return ret;
    }

    void
    MatchPaletteDither(Image* image, const Palette& palette)
    {
        struct Error
        {
            float r = 0;
            float g = 0;
            float b = 0;
        };
        auto errors = Table<Error>::FromWidthHeight(image->GetWidth(), image->GetHeight());
        const auto errors_range = errors.Indices();

        *image = NewImageFrom(*image, [&](int x, int y)
        {
            auto       pixel       = image->GetPixel(x, y);
            auto       new_color   = crgb(pixel);
            const auto pixel_error = errors(x, y);
            new_color.r += pixel_error.r;
            new_color.g += pixel_error.g;
            new_color.b += pixel_error.b;
            new_color                = clamp(new_color);
            const auto palette_color = palette.GetClosestColor(crgbi(new_color));

            const auto pcf = crgb(palette_color);
            const auto error = Error
            {
                new_color.r - pcf.r,
                new_color.g - pcf.g,
                new_color.b - pcf.b
            };
            const auto floyd_steinberg = std::vector<std::pair<vec2i, float>>
            {
                {vec2i(1, 0), 7.0f / 16.0f},
                {vec2i(1, -1), 1.0f / 16.0f},
                {vec2i(0, -1), 5.0f / 16.0f},
                {vec2i(-1, -1), 3.0f / 16.0f}
            };

            for(auto fs: floyd_steinberg)
            {
                auto nx = static_cast<int>(x) + fs.first.x;
                auto ny = static_cast<int>(y) + fs.first.y;
                auto factor = fs.second;
                if(errors_range.ContainsInclusive(nx, ny))
                {
                    auto& e = errors(nx, ny);
                    e.r += factor * error.r;
                    e.g += factor * error.g;
                    e.b += factor * error.b;
                }
            }

            return rgbai(palette_color, pixel.a);
        });
    }

    vec3f
    Cvec3(const rgb f)
    {
        return {f.r, f.g, f.b};
    }

    vec3f
    Cvec3(const rgbi c)
    {
        return Cvec3(crgb(c));
    }

    vec3f
    Cvec3(const rgbai c)
    {
        return Cvec3(crgb(c));
    }

    void
    EdgeDetection(Image* image, float r)
    {
        *image = NewImageFrom(*image, [&](int x, int y) {
            const auto pixel = Cvec3(image->GetPixel(x, y));
            const auto top
                    = y == image->GetHeight() - 1
                              ? false
                              : (pixel - Cvec3(image->GetPixel(x, y + 1)))
                                                .GetLength()
                                        >= r;
            const auto left
                    = x == 0 ? false
                             : (pixel - Cvec3(image->GetPixel(x - 1, y)))
                                               .GetLength()
                                       >= r;
            const bool edge = top || left;
            const auto c    = edge ? Color::White : Color::Black;
            return rgbai(c, 255);
        });
    }


    void
    ColorDetection(Image* image, rgb color, float r)
    {
        const auto basis = Cvec3(color);
        image->Filter([&](const rgbai pixel) {
            const auto check = (Cvec3(pixel) - basis).GetLength() <= r;
            const auto c     = check ? Color::White : Color::Black;
            return rgbai(c, 255);
        });
    }

    template <typename C>
    void
    LutTransform(Image* image, C c)
    {
        std::vector<unsigned char> lut;
        lut.reserve(256);
        for(int i = 0; i < 256; i++)
        {
            lut.emplace_back(c(i));
        }
        image->Filter([&](const rgbai pixel) {
            return rgbai(
                    rgbi(lut[pixel.r], lut[pixel.g], lut[pixel.b]), pixel.a);
        });
    }

    void
    ChangeBrightness(Image* image, int change)
    {
        LutTransform(image, [&](int i) {
            return KeepWithin(MakeRange(0, 255), i + change);
        });
    }

    void
    ChangeContrast(Image* image, float contrast)
    {
        const auto tc = tan(contrast);
        LutTransform(image, [&](int i) {
            const auto a = 128.0f + 128.0f * tc;
            const auto b = 128.0f - 128.0f * tc;
            if(i < a && b < i)
            {
                return static_cast<int>((i - 128) / tc + 128);
            }
            else if(i > a)
            {
                return 255;
            }
            else
            {
                return 0;
            }
        });
    }

}  // namespace euphoria::core
