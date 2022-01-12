#include "core/imagefilters.h"

#include "core/image.h"
#include "core/palette.h"
#include "core/table.h"
#include "core/vec3.h"


namespace euphoria::core
{
    namespace
    {
        Rgbai
        make_gray(unsigned char g, unsigned char a)
        {
            return {Rgbi {g, g, g}, a};
        }
    }


    //
    // https://twitter.com/FreyaHolmer/status/1116502994684530688
    Rgbai
    make_grayscale(Rgbai c, Grayscale grayscale)
    {
        switch(grayscale)
        {
        case Grayscale::r: return make_gray(c.r, c.a);
        case Grayscale::g: return make_gray(c.g, c.a);
        case Grayscale::b: return make_gray(c.b, c.a);
        case Grayscale::a: return make_gray(c.a, c.a);
        case Grayscale::max: return make_gray(max(c.r, max(c.g, c.b)), c.a);
        case Grayscale::gamma:
            {
                const auto d = dot(crgb(c), Rgb(0.22f, 0.707f, 0.071f));
                return Rgbai(crgbi(Rgb(d)), c.a);
            }
        case Grayscale::linear:
            {
                const auto d = dot(crgb(c), Rgb(0.0397f, 0.4580f, 0.0061f));
                return Rgbai(crgbi(Rgb(d)), c.a);
            }
        case Grayscale::average:
            {
                const auto cc = crgb(c);
                const auto g = (cc.r + cc.g + cc.b) / 3;
                return Rgbai(crgbi(Rgb(g)), c.a);
            }

        default:
            return make_gray(c.a, c.a);
        }
    }


    void
    make_grayscale(Image* image, Grayscale grayscale)
    {
        image->filter([grayscale](const Rgbai& c)
        {
            return make_grayscale(c, grayscale);
        });
    }

    void
    match_palette(Image* image, const Palette& palette)
    {
        image->filter([&palette](const Rgbai& c) {
            const auto cc = crgbi(c);
            const auto nc = palette.get_closest_color(cc);

            return Rgbai(nc, c.a);
        });
    }

    template <typename C>
    [[nodiscard]] Image
    create_new_image_from(const Image& image_source, C callback)
    {
        Image ret;
        if(image_source.has_alpha)
        {
            ret.setup_with_alpha_support(image_source.width, image_source.height, -1);
        }
        else
        {
            ret.setup_no_alpha_support(image_source.width, image_source.height, -1);
        }
        ret.set_all_top_bottom(callback);
        return ret;
    }

    void
    match_palette_dither(Image* image, const Palette& palette)
    {
        struct ColorError
        {
            float r = 0;
            float g = 0;
            float b = 0;
        };
        auto errors = Table<ColorError>::from_width_height(image->width, image->height);
        const auto errors_range = errors.get_indices();

        *image = create_new_image_from(*image, [&](int x, int y)
        {
            auto pixel = image->get_pixel(x, y);
            auto new_color = crgb(pixel);
            const auto pixel_error = errors(x, y);
            new_color.r += pixel_error.r;
            new_color.g += pixel_error.g;
            new_color.b += pixel_error.b;
            new_color = clamp(new_color);
            const auto palette_color = palette.get_closest_color(crgbi(new_color));

            const auto pcf = crgb(palette_color);
            const auto error = ColorError
            {
                new_color.r - pcf.r,
                new_color.g - pcf.g,
                new_color.b - pcf.b
            };
            const auto floyd_steinberg = std::vector<std::pair<Vec2i, float>>
            {
                {Vec2i(1, 0), 7.0f / 16.0f},
                {Vec2i(1, -1), 1.0f / 16.0f},
                {Vec2i(0, -1), 5.0f / 16.0f},
                {Vec2i(-1, -1), 3.0f / 16.0f}
            };

            for(auto fs: floyd_steinberg)
            {
                auto nx = x + fs.first.x;
                auto ny = y + fs.first.y;
                auto factor = fs.second;
                if(errors_range.contains_inclusive(nx, ny))
                {
                    auto& e = errors(nx, ny);
                    e.r += factor * error.r;
                    e.g += factor * error.g;
                    e.b += factor * error.b;
                }
            }

            return Rgbai(palette_color, pixel.a);
        });
    }

    Vec3f
    c_vec3(const Rgb f)
    {
        return {f.r, f.g, f.b};
    }

    Vec3f
    c_vec3(const Rgbi c)
    {
        return c_vec3(crgb(c));
    }

    Vec3f
    c_vec3(const Rgbai c)
    {
        return c_vec3(crgb(c));
    }

    void
    edge_detection(Image* image, float r)
    {
        *image = create_new_image_from(*image, [&](int x, int y) {
            const auto pixel = c_vec3(image->get_pixel(x, y));
            const auto top
                    = y == image->height - 1
                              ? false
                              : (pixel - c_vec3(image->get_pixel(x, y + 1)))
                                                .get_length()
                                        >= r;
            const auto left
                    = x == 0 ? false
                             : (pixel - c_vec3(image->get_pixel(x - 1, y)))
                                               .get_length()
                                       >= r;
            const bool edge = top || left;
            const auto c = edge ? NamedColor::white : NamedColor::black;
            return Rgbai(c, 255);
        });
    }


    void
    color_detection(Image* image, Rgb color, float r)
    {
        const auto basis = c_vec3(color);
        image->filter([&](const Rgbai pixel) {
            const auto check = (c_vec3(pixel) - basis).get_length() <= r;
            const auto c = check ? NamedColor::white : NamedColor::black;
            return Rgbai(c, 255);
        });
    }

    template <typename C>
    void
    run_lut_transform(Image* image, C c)
    {
        std::vector<unsigned char> lut;
        lut.reserve(256);
        for(int i = 0; i < 256; i++)
        {
            lut.emplace_back(c(i));
        }
        image->filter([&](const Rgbai pixel) {
            return Rgbai(
                    Rgbi(lut[pixel.r], lut[pixel.g], lut[pixel.b]), pixel.a);
        });
    }

    void
    change_brightness(Image* image, int change)
    {
        run_lut_transform(image, [&](int i) {
            return keep_within(make_range(0, 255), i + change);
        });
    }

    void
    change_contrast(Image* image, const Angle& contrast)
    {
        const auto tc = tan(contrast);
        run_lut_transform(image, [&](int i) {
            const auto f = c_int_to_float(i);
            const auto a = 128.0f + 128.0f * tc;
            const auto b = 128.0f - 128.0f * tc;
            if(f < a && b < f)
            {
                return c_float_to_int((f - 128.0f) / tc) + 128;
            }
            else if(f > a)
            {
                return 255;
            }
            else
            {
                return 0;
            }
        });
    }

}
