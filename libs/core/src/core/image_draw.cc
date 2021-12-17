#include "core/image_draw.h"

#include "core/numeric.h"
#include "core/assert.h"
#include "core/range.h"
#include "core/loadedfont.h"
#include "core/utf8.h"
#include "core/rgb_blend.h"
#include "core/minmax.h"
#include "core/intersection.h"
#include "core/minmax.h"

#include "core/log.h"

#include <utility>
#include <cmath>

namespace euphoria::core
{
    Recti
    whole_image(const Image& image)
    {
        return Recti::from_top_left_width_height
        (
            Vec2i{0, image.height},
            image.width,
            image.height
        );
    }

    void
    clear(Image* image, const Rgbai& color)
    {
        ASSERT(image);
        return draw_rect(image, color, whole_image(*image));
    }

    void
    draw_rect(Image* image, const Rgbai& color, const Recti& rect)
    {
        ASSERT(image);
        const int left = rect.get_top_left().x;
        const int right = rect.get_top_right().x;
        const int top = rect.get_top_left().y;
        const int bottom = rect.get_bottom_left().y;
        // ASSERTX(left >= 0, left);
        // ASSERTX(bottom >= 0, bottom);
        for(int y = bottom; y < top; ++y)
        {
            if(y < 0 || y >= image->height)
            {
                continue;
            }
            for(int x = left; x < right; ++x)
            {
                if(x < 0 || x >= image->width)
                {
                    continue;
                }
                image->set_pixel(x, y, color);
            }
        }
    }


    void
    draw_square(Image* image, const Rgbai& color, int x, int y, int size)
    {
        ASSERT(image);
        draw_rect
        (
            image,
            color,
            // is the +1 right?
            Recti::from_top_left_width_height(Vec2i{x, y + 1}, size, size)
        );
    }


    namespace
    {
        Rectf
        calculate_bounding_rect(const std::vector<Vec2f>& poly)
        {
            const auto [min, max] = find_min_max<Vec2f>
            (
                poly,
                [](const auto& lhs, const auto& rhs)
                {
                    return Vec2f
                    {
                        std::min(lhs.x, rhs.x),
                        std::min(lhs.y, rhs.y)
                    };
                },
                [](const auto& lhs, const auto& rhs)
                {
                    return Vec2f
                    {
                        std::max(lhs.x, rhs.x),
                        std::max(lhs.y, rhs.y)
                    };
                }
            );

            return Rectf::from_left_right_bottom_top(min.x, max.x, min.y, max.y);
        }

        bool
        does_ray_intersect_segment(const Vec2f& u, const Vec2f& a, const Vec2f& b)
        {
            // todo(Gustav): move to math
            return
                (a.y > u.y) != (b.y > u.y) &&
                u.x < (b.x - a.x) * (u.y - a.y) / (b.y - a.y) + a.x
                ;
        }

        bool
        point_is_in_poly(const Vec2f& p, const std::vector<Vec2f>& poly)
        {
            // todo(Gustav): make pretty and move to custom struct
            if(poly.size() < 3)
            {
                return false;
            }

            auto a = poly[0];
            auto in = does_ray_intersect_segment(p, *poly.rbegin(), a);

            for(auto i = poly.begin() + 1; i != poly.end(); ++i)
            {
                const auto b = *i;
                if(does_ray_intersect_segment(p, a, b))
                {
                    in = !in;
                }
                a = b;
            }

            return in;
        }
    }

    void
    fill_poly(Image* image, const Rgbai& color, const std::vector<Vec2f>& poly)
    {
        ASSERT(image);

        const auto rect = calculate_bounding_rect(poly);
        const int left = c_float_to_int(rect.get_top_left().x);
        const int right = c_float_to_int(rect.get_top_right().x);
        const int top = c_float_to_int(rect.get_top_left().y);
        const int bottom = c_float_to_int(rect.get_bottom_left().y);

        // ASSERTX(left >= 0, left);
        // ASSERTX(bottom >= 0, bottom);
        for(int y = bottom; y < top; ++y)
        {
            if(y < 0 || y >= image->height) { continue; }
            for(int x = left; x < right; ++x)
            {
                if(x < 0 || x >= image->width) { continue; }

                if(point_is_in_poly(Vec2f{c_int_to_float(x), c_int_to_float(y)}, poly))
                {
                    image->set_pixel(x, y, color);
                }
            }
        }
    }

    void
    draw_circle
    (
        Image* image,
        const Rgb& color,
        const Vec2i& center,
        float radius,
        float softness,
        float inner
    )
    {
        ASSERT(image);
        const int left = max(0, center.x - floor_to_int(radius - softness));
        const int right = min
        (
            image->width,
            ceil_to_int(c_int_to_float(center.x) + radius + softness)
        );
        const int top = max(0, floor_to_int(c_int_to_float(center.y) - radius - softness));
        const int bottom = min
        (
            image->height,
            center.y + ceil_to_int(radius + softness)
        );

        // color modes
        // nothing INNER-SOFTNESS fade INNER full RADIUS fade RADIUS+SOFTNESS nothing

        for(int y = top; y < bottom; ++y)
        {
            for(int x = left; x < right; ++x)
            {
                // todo(Gustav): use length squared!
                const float sq = Vec2f::from_to
                (
                    Vec2f{static_cast<float>(x), static_cast<float>(y)},
                    static_cast<Vec2f>(center)
                ).get_length();
                bool blend = false;
                float blend_factor = 1.0f;

                const auto a = make_range(inner - softness, inner);
                const auto b = make_range(radius, radius + softness);

                if(is_within(a, sq))
                {
                    blend_factor = to01(a, sq);
                    blend = true;
                }
                else if(is_within(b, sq))
                {
                    blend_factor = 1.0f - to01(b, sq);
                    blend = true;
                }
                else if(is_within(make_range(inner, radius), sq))
                {
                    // full color
                }
                else
                {
                    // outside
                    continue;
                }

                const Rgb paint_color = blend
                    ? RgbTransform::Transform
                    (
                        crgb(image->get_pixel(x, y)),
                        blend_factor,
                        color
                    )
                    : color
                    ;

                image->set_pixel(x, y, Rgbai{paint_color});
            }
        }
    }

    void
    draw_line_fast
    (
        Image* image,
        const Rgbai& color,
        const Vec2i& from,
        const Vec2i& to
    )
    {
        ASSERT(image);

        // reference:
        // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

        auto plot = [&](int x, int y)
        {
            image->set_pixel(x, y, color);
        };

        auto plot_line_low = [&](int x0, int y0, int x1, int y1)
        {
            auto dx = x1 - x0;
            auto dy = y1 - y0;
            auto yi = 1;

            if(dy < 0)
            {
                yi = -1;
                dy = -dy;
            }

            auto difference = 2 * dy - dx;
            auto y = y0;

            for(int x=x0; x<x1; x+=1)
            {
                plot(x,y);
                if(difference > 0)
                {
                    y = y + yi;
                    difference = difference - 2 * dx;
                }
                difference = difference + 2 * dy;
            }
        };

        auto plot_line_high = [&](int x0, int y0, int x1, int y1)
        {
            int dx = x1 - x0;
            int dy = y1 - y0;
            int xi = 1;
            if(dx < 0)
            {
                xi = -1;
                dx = -dx;
            }
            int difference = 2 * dx - dy;
            int x = x0;

            for(int y=y0; y<y1; y+=1)
            {
                plot(x,y);
                if(difference > 0)
                {
                    x = x + xi;
                    difference = difference - 2 * dy;
                }
                difference = difference + 2 * dx;
            }
        };

        const auto x0 = from.x;
        const auto y0 = from.y;
        const auto x1 = to.x;
        const auto y1 = to.y;
        if(std::abs(y1 - y0) < std::abs(x1 - x0))
        {
            if(x0 > x1) { plot_line_low(x1, y1, x0, y0); }
            else        { plot_line_low(x0, y0, x1, y1); }
        }
        else
        {
            if(y0 > y1) { plot_line_high(x1, y1, x0, y0); }
            else        { plot_line_high(x0, y0, x1, y1); }
        }
    }


    void
    draw_line_antialiased
    (
        Image* image,
        const Rgb& color,
        const Vec2i& from,
        const Vec2i& to
    )
    {
        ASSERT(image);
        return draw_line_antialiased
        (
             image,
             color,
             static_cast<Vec2f>(from),
             static_cast<Vec2f>(to)
        );
    }


    void
    draw_line_antialiased
    (
        Image* image,
        const Rgb& color,
        const Vec2f& from,
        const Vec2f& to
    )
    {
        // reference:
        // https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm

        auto ipart = [&](float x) -> int { return floor_to_int(x); };
        auto round = [&](float x) -> int { return ipart(x + 0.5f); };
        auto fpart = [&](float x) -> float { return x - std::floor(x); };
        auto rfpart = [&](float x) -> float { return 1.0f - fpart(x); };

        auto plot = [&](int x, int y, float c)
        {
            // plot the pixel at (x, y) with brightness c (where 0 ≤ c ≤ 1)
            const bool valid_x = is_within_inclusive_as_int(0, x, image->width - 1);
            const bool valid_y = is_within_inclusive_as_int(0, y, image->height - 1);
            if(valid_x && valid_y)
            {
                const Rgb paint_color = RgbTransform::Transform
                (
                    crgb(image->get_pixel(x, y)),
                    c,
                    color
                );
                image->set_pixel(x, y, Rgbai{paint_color});
            }
        };

        ASSERT(image);

        float x0 = from.x;
        float y0 = from.y;
        float x1 = to.x;
        float y1 = to.y;

        using std::swap;

        auto steep = abs(y1 - y0) > abs(x1 - x0);

        if(steep)
        {
            swap(x0, y0);
            swap(x1, y1);
        }
        if(x0 > x1)
        {
            swap(x0, x1);
            swap(y0, y1);
        }

        float dx = x1 - x0;
        float dy = y1 - y0;
        float gradient = dy / dx;
        if(dx == 0.0f)
        {
            gradient = 1.0f;
        }

        // handle first endpoint
        auto xend = round(x0);
        auto yend = y0 + gradient * (c_int_to_float(xend) - x0);
        auto xgap = rfpart(x0 + 0.5f);
        auto xpxl1 = xend; // this will be used in the main loop
        auto ypxl1 = ipart(yend);
        if(steep)
        {
            plot(ypxl1,   xpxl1, rfpart(yend) * xgap);
            plot(ypxl1+1, xpxl1,  fpart(yend) * xgap);
        }
        else
        {
            plot(xpxl1, ypxl1  , rfpart(yend) * xgap);
            plot(xpxl1, ypxl1+1,  fpart(yend) * xgap);
        }
        auto intery = yend + gradient; // first y-intersection for the main loop

        // handle second endpoint
        xend = round(x1);
        yend = y1 + gradient * (c_int_to_float(xend) - x1);
        xgap = fpart(x1 + 0.5f);
        auto xpxl2 = xend; //this will be used in the main loop
        auto ypxl2 = ipart(yend);

        if(steep)
        {
            plot(ypxl2  , xpxl2, rfpart(yend) * xgap);
            plot(ypxl2+1, xpxl2,  fpart(yend) * xgap);
        }
        else
        {
            plot(xpxl2, ypxl2,  rfpart(yend) * xgap);
            plot(xpxl2, ypxl2+1, fpart(yend) * xgap);
        }

        // main loop
        if(steep)
        {
            for(auto x=xpxl1 + 1; x<=xpxl2 - 1; x+=1)
            {
                plot(ipart(intery)  , x, rfpart(intery));
                plot(ipart(intery)+1, x,  fpart(intery));
                intery = intery + gradient;
            }
        }
        else
        {
            for(auto x = xpxl1 + 1; x<xpxl2 - 1; x+=1)
            {
                plot(x, ipart(intery),  rfpart(intery));
                plot(x, ipart(intery)+1, fpart(intery));
                intery = intery + gradient;
            }
        }
    }

    Rgba tint_color(const Rgba& c, const Rgb& tint)
    {
        return {{c.r * tint.r, c.g * tint.g, c.b * tint.b}, c.a};
    }

    Rgbai tint_color(const Rgbai& c, const Rgbai& tint)
    {
        return crgbai(tint_color(crgba(c), crgb(tint)));
    }

    void
    simple_image_blend
    (
        Image* dst,
        const Vec2i& p,
        const Image& src,
        const Rgbai& tint
    )
    {
        for(int y=0; y<src.height; y+=1)
        {
            for(int x=0; x<src.width; x+=1)
            {
                const auto dx = p.x + x;
                const auto dy = p.y + y;
                if(dx >= dst->width) { continue; }
                if(dy >= dst->height) { continue; }
                const auto dst_color = dst->get_pixel(dx, dy);
                const auto src_color = src.get_pixel(x, y);
                const auto tinted_color = tint_color(src_color, tint);
                const auto result_color = blend(tinted_color, dst_color);
                dst->set_pixel(dx, dy, result_color);
            }
        }
    }

    void
    draw_text
    (
        Image* image,
        const Vec2i& start_pos,
        const std::string& text,
        const Rgbai& color,
        const LoadedFont& font
    )
    {
        ASSERT(image);

        Vec2i pos = start_pos;
        utf8_to_codepoints(text, [&](int cp)
        {
            if(cp == '\n')
            {
                pos.x = start_pos.x;
                // todo(Gustav): font needs lineheight...
                pos.y -= font.line_height;
            }
            else
            {
                const auto glyph_found = font.codepoint_to_glyph.find(cp);
                if(glyph_found != font.codepoint_to_glyph.end())
                {
                    const auto& glyph = glyph_found->second;
                    simple_image_blend(image, pos, glyph.image, color);
                    pos.x += glyph.advance;
                }
            }
        });
    }

    void
    paste_image
    (
        Image* dest_image,
        const Vec2i& position,
        const Image& source_image,
        BlendMode blend_mode,
        PixelsOutside clip
    )
    {
        ASSERT(dest_image);

        for(int y = 0; y < source_image.height; ++y)
        {
            for(int x = 0; x < source_image.width; ++x)
            {
                const auto dest_x = position.x + x;
                const auto dest_y = position.y + y;
                if
                (
                    clip == PixelsOutside::discard &&
                    is_within(dest_image->get_indices(), Vec2i(dest_x, dest_y)) == false
                )
                {
                    // nop
                }
                else
                {
                    const auto top = source_image.get_pixel(x, y);
                    const auto bottom = dest_image->get_pixel(dest_x, dest_y);
                    const auto color = blend(top, bottom, blend_mode);
                    dest_image->set_pixel(dest_x, dest_y, color);
                }
            }
        }
    }


    void
    fill_triangle
    (
        Image* image,
        const Vec2f& a,
        const Vec2f& b,
        const Vec2f& c,
        const Rgbai& color
    )
    {
        const auto [minf, maxf] = find_min_max<Vec2f, std::vector<Vec2f> >
        (
            {a, b, c},
            [](const Vec2f& lhs, const Vec2f& rhs) -> Vec2f
            {
                return {std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y)};
            },
            [](const Vec2f& lhs, const Vec2f& rhs) -> Vec2f
            {
                return {std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y)};
            }
        );

        const auto min = static_cast<Vec2i>(minf);
        const auto max = static_cast<Vec2i>(maxf);

        for(int y=min.y; y<=max.y; y+=1)
        {
            for(int x=min.x; x<=max.x; x+=1)
            {
                const bool valid_x = is_within_inclusive_as_int
                (
                    0, x, image->width - 1
                );
                const bool valid_y = is_within_inclusive_as_int
                (
                    0, y, image->height - 1
                );
                if(valid_x && valid_y)
                {
                    const auto inside_triangle = is_point_in_triangle
                    (
                        a,
                        b,
                        c,
                        Vec2f{static_cast<float>(x), static_cast<float>(y)}
                    );
                    if(inside_triangle)
                    {
                        image->set_pixel(x, y, color);
                    }
                }
            }
        }
    }


    void
    draw_arrow
    (
        Image* image,
        const Vec2f& from,
        const Vec2f& to,
        const Rgbai& color,
        float size
    )
    {
        // based on code from https://www.codeproject.com/Questions/125049/Draw-an-arrow-with-big-cap
        // todo(Gustav): this is too complicated, and hard to customize, different pointy arrows that ain't 90 degrees
        // there must be a better way to do it
        // also generalize it so we can have arrows in dvg/dummper code too
        const Vec2f arrow_point = to;

        const auto arrow_length = sqrt(square(abs(from.x - to.x)) +
                                       square(abs(from.y - to.y)));

        const auto arrow_angle = atan2(abs(from.y - to.y), abs(from.x - to.x));
        const auto angle_b = atan2((3 * size), (arrow_length - (3 * size)));
        const auto secondary_length = (3 * size) / sin(angle_b);

        auto angle_c = Angle::from_degrees(90) - arrow_angle - angle_b;
        const auto arrow_point_left_x = from.x > to.x
            ? from.x - (sin(angle_c) * secondary_length)
            : (sin(angle_c) * secondary_length) + from.x
            ;
        const auto arrow_point_left_y = from.y > to.y
            ? from.y - (cos(angle_c) * secondary_length)
            : (cos(angle_c) * secondary_length) + from.y
            ;
        const auto arrow_point_left = Vec2f
        {
            arrow_point_left_x,
            arrow_point_left_y
        };

        //move to the right point
        angle_c = arrow_angle - angle_b;

        const auto arrow_point_right_x = from.x > to.x
            ? from.x - (cos(angle_c) * secondary_length)
            : (cos(angle_c) * secondary_length) + from.x
            ;
        const auto arrow_point_right_y = from.y > to.y
            ? from.y - (sin(angle_c) * secondary_length)
            : (sin(angle_c) * secondary_length) + from.y
            ;
        const auto arrow_point_right = Vec2f
        {
            arrow_point_right_x,
            arrow_point_right_y
        };

        // line
        draw_line_antialiased(image, crgb(color), from, to);

        // wings
        // DrawLineAntialiased(image, rgb(color), arrowPoint, arrowPointLeft);
        // DrawLineAntialiased(image, rgb(color), arrowPoint, arrowPointRight);
        // DrawLineAntialiased(image, rgb(color), arrowPointLeft, arrowPointRight);

        fill_triangle(image, arrow_point_left, arrow_point, arrow_point_right, color);
    }


}

