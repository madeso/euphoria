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
    WholeImage(const Image& image)
    {
        return Recti::FromTopLeftWidthHeight
        (
            vec2i{0, image.GetHeight()},
            image.GetWidth(),
            image.GetHeight()
        );
    }

    void
    Clear(Image* image, const rgbai& color)
    {
        ASSERT(image);
        return DrawRect(image, color, WholeImage(*image));
    }

    void
    DrawRect(Image* image, const rgbai& color, const Recti& rect)
    {
        ASSERT(image);
        const int left = rect.TopLeft().x;
        const int right = rect.TopRight().x;
        const int top = rect.TopLeft().y;
        const int bottom = rect.BottomLeft().y;
        // ASSERTX(left >= 0, left);
        // ASSERTX(bottom >= 0, bottom);
        for(int y = bottom; y < top; ++y)
        {
            if(y < 0 || y >= image->GetHeight())
            {
                continue;
            }
            for(int x = left; x < right; ++x)
            {
                if(x < 0 || x >= image->GetWidth())
                {
                    continue;
                }
                image->SetPixel(x, y, color);
            }
        }
    }


    void
    DrawSquare(Image* image, const rgbai& color, int x, int y, int size)
    {
        ASSERT(image);
        DrawRect
        (
            image,
            color,
            // is the +1 right?
            Recti::FromTopLeftWidthHeight(vec2i{x, y + 1}, size, size)
        );
    }


    namespace
    {
        Rectf
        BoundingRect(const std::vector<vec2f>& poly)
        {
            const auto [min, max] = FindMinMax<vec2f>
            (
                poly,
                [](const auto& lhs, const auto& rhs)
                {
                    return vec2f
                    {
                        std::min(lhs.x, rhs.x),
                        std::min(lhs.y, rhs.y)
                    };
                },
                [](const auto& lhs, const auto& rhs)
                {
                    return vec2f
                    {
                        std::max(lhs.x, rhs.x),
                        std::max(lhs.y, rhs.y)
                    };
                }
            );

            return Rectf::FromLeftRightBottomTop(min.x, max.x, min.y, max.y);
        }

        bool
        RayIntersectsSegment(const vec2f& u, const vec2f& a, const vec2f& b)
        {
            // todo(Gustav): move to math
            return
                (a.y > u.y) != (b.y > u.y) &&
                u.x < (b.x - a.x) * (u.y - a.y) / (b.y - a.y) + a.x
                ;
        }

        bool
        PointInPoly(const vec2f& p, const std::vector<vec2f>& poly)
        {
            // todo(Gustav): make pretty and move to custom struct
            if(poly.size() < 3)
            {
                return false;
            }

            auto a = poly[0];
            auto in = RayIntersectsSegment(p, *poly.rbegin(), a);

            for(auto i = poly.begin() + 1; i != poly.end(); ++i)
            {
                const auto b = *i;
                if(RayIntersectsSegment(p, a, b))
                {
                    in = !in;
                }
                a = b;
            }

            return in;
        }
    }

    void
    FillPoly(Image* image, const rgbai& color, const std::vector<vec2f>& poly)
    {
        ASSERT(image);

        const auto rect = BoundingRect(poly);
        const int left = rect.TopLeft().x;
        const int right = rect.TopRight().x;
        const int top = rect.TopLeft().y;
        const int bottom = rect.BottomLeft().y;

        // ASSERTX(left >= 0, left);
        // ASSERTX(bottom >= 0, bottom);
        for(int y = bottom; y < top; ++y)
        {
            if(y < 0 || y >= image->GetHeight()) { continue; }
            for(int x = left; x < right; ++x)
            {
                if(x < 0 || x >= image->GetWidth()) { continue; }

                if(PointInPoly(vec2f(x, y), poly))
                {
                    image->SetPixel(x, y, color);
                }
            }
        }
    }

    void
    DrawCircle
    (
        Image* image,
        const rgb& color,
        const vec2i& center,
        float radius,
        float softness,
        float inner
    )
    {
        ASSERT(image);
        const int left = Max(0, Floori(center.x - radius - softness));
        const int right = Min
        (
            image->GetWidth(),
            Ceili(center.x + radius + softness)
        );
        const int top = Max(0, Floori(center.y - radius - softness));
        const int bottom = Min
        (
            image->GetHeight(),
            Ceili(center.y + radius + softness)
        );

        // color modes
        // nothing INNER-SOFTNESS fade INNER full RADIUS fade RADIUS+SOFTNESS nothing

        for(int y = top; y < bottom; ++y)
        {
            for(int x = left; x < right; ++x)
            {
                // todo(Gustav): use length squared!
                const float sq = vec2f::FromTo
                (
                    vec2f{static_cast<float>(x), static_cast<float>(y)},
                    center.StaticCast<float>()
                ).GetLength();
                bool blend = false;
                float blend_factor = 1.0f;

                const auto a = MakeRange(inner - softness, inner);
                const auto b = MakeRange(radius, radius + softness);

                if(IsWithin(a, sq))
                {
                    blend_factor = To01(a, sq);
                    blend = true;
                }
                else if(IsWithin(b, sq))
                {
                    blend_factor = 1.0f - To01(b, sq);
                    blend = true;
                }
                else if(IsWithin(MakeRange(inner, radius), sq))
                {
                    // full color
                }
                else
                {
                    // outside
                    continue;
                }

                const rgb paint_color = blend
                    ? rgb_transform::Transform
                    (
                        crgb(image->GetPixel(x, y)),
                        blend_factor,
                        color
                    )
                    : color
                    ;

                image->SetPixel(x, y, rgbai{paint_color});
            }
        }
    }

    void
    DrawLineFast
    (
        Image* image,
        const rgbai& color,
        const vec2i& from,
        const vec2i& to
    )
    {
        ASSERT(image);

        // reference:
        // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

        auto plot = [&](int x, int y)
        {
            image->SetPixel(x, y, color);
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

            auto D = 2*dy - dx;
            auto y = y0;

            for(int x=x0; x<x1; x+=1)
            {
                plot(x,y);
                if(D > 0)
                {
                    y = y + yi;
                    D = D - 2*dx;
                }
                D = D + 2*dy;
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
            int D = 2*dx - dy;
            int x = x0;

            for(int y=y0; y<y1; y+=1)
            {
                plot(x,y);
                if(D > 0)
                {
                    x = x + xi;
                    D = D - 2*dy;
                }
                D = D + 2*dx;
            }
        };

        const auto x0 = from.x;
        const auto y0 = from.y;
        const auto x1 = to.x;
        const auto y1 = to.y;
        if(abs(y1 - y0) < abs(x1 - x0))
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
    DrawLineAntialiased
    (
        Image* image,
        const rgb& color,
        const vec2i& from,
        const vec2i& to
    )
    {
        ASSERT(image);
        return DrawLineAntialiased
        (
             image,
             color,
             from.StaticCast<float>(),
             to.StaticCast<float>()
        );
    }


    void
    DrawLineAntialiased
    (
        Image* image,
        const rgb& color,
        const vec2f& from,
        const vec2f& to
    )
    {
        // reference:
        // https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm

        auto ipart = [&](float x) -> int { return Floori(x); };
        auto round = [&](float x) -> int { return ipart(x + 0.5f); };
        auto fpart = [&](float x) -> float { return x - std::floor(x); };
        auto rfpart = [&](float x) -> float { return 1.0f - fpart(x); };

        auto plot = [&](int x, int y, float c)
        {
            // plot the pixel at (x, y) with brightness c (where 0 ≤ c ≤ 1)
            const bool valid_x = IsWithinInclusivei(0, x, image->GetWidth() - 1);
            const bool valid_y = IsWithinInclusivei(0, y, image->GetHeight() - 1);
            if(valid_x && valid_y)
            {
                const rgb paint_color = rgb_transform::Transform
                (
                    crgb(image->GetPixel(x, y)),
                    c,
                    color
                );
                image->SetPixel(x, y, rgbai{paint_color});
            }
        };

        ASSERT(image);

        float x0 = from.x;
        float y0 = from.y;
        float x1 = to.x;
        float y1 = to.y;

        using std::swap;

        auto steep = Abs(y1 - y0) > Abs(x1 - x0);

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
        auto yend = y0 + gradient * (xend - x0);
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
        yend = y1 + gradient * (xend - x1);
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

    rgba Tint(const rgba& c, const rgb& tint)
    {
        return {{c.r * tint.r, c.g * tint.g, c.b * tint.b}, c.a};
    }

    rgbai Tint(const rgbai& c, const rgbai& tint)
    {
        return crgbai(Tint(crgba(c), crgb(tint)));
    }

    void
    SimpleImageBlend
    (
        Image* dst,
        const vec2i& p,
        const Image& src,
        const rgbai& tint
    )
    {
        for(int y=0; y<src.GetHeight(); y+=1)
        {
            for(int x=0; x<src.GetWidth();  x+=1)
            {
                const auto dx = p.x + x;
                const auto dy = p.y + y;
                if(dx >= dst->GetWidth()) { continue; }
                if(dy >= dst->GetHeight()) { continue; }
                const auto dst_color = dst->GetPixel(dx, dy);
                const auto src_color = src.GetPixel(x, y);
                const auto tinted_color = Tint(src_color, tint);
                const auto result_color = Blend(tinted_color, dst_color);
                dst->SetPixel(dx, dy, result_color);
            }
        }
    }

    void
    DrawText
    (
        Image* image,
        const vec2i& start_pos,
        const std::string& text,
        const rgbai& color,
        const LoadedFont& font
    )
    {
        ASSERT(image);

        vec2i pos = start_pos;
        Utf8ToCodepoints(text, [&](unsigned int cp)
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
                    SimpleImageBlend(image, pos, glyph.image, color);
                    pos.x += glyph.advance;
                }
            }
        });
    }

    void
    PasteImage
    (
        Image* dest_image,
        const vec2i& position,
        const Image& source_image,
        BlendMode blend_mode,
        PixelsOutside clip
    )
    {
        ASSERT(dest_image);

        for(int y = 0; y < source_image.GetHeight(); ++y)
        {
            for(int x = 0; x < source_image.GetWidth(); ++x)
            {
                const auto dest_x = position.x + x;
                const auto dest_y = position.y + y;
                if
                (
                    clip == PixelsOutside::Discard &&
                    IsWithin(dest_image->GetIndices(), vec2i(dest_x, dest_y)) == false
                )
                {
                    // nop
                }
                else
                {
                    const auto top = source_image.GetPixel(x, y);
                    const auto bottom = dest_image->GetPixel(dest_x, dest_y);
                    const auto color = Blend(top, bottom, blend_mode);
                    dest_image->SetPixel(dest_x, dest_y, color);
                }
            }
        }
    }


    void
    FillTriangle
    (
        Image* image,
        const vec2f& a,
        const vec2f& b,
        const vec2f& c,
        const rgbai& color
    )
    {
        const auto [minf, maxf] = FindMinMax<vec2f, std::vector<vec2f> >
        (
            {a, b, c},
            [](const vec2f& lhs, const vec2f& rhs) -> vec2f
            {
                return {std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y)};
            },
            [](const vec2f& lhs, const vec2f& rhs) -> vec2f
            {
                return {std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y)};
            }
        );

        const auto min = minf.StaticCast<int>();
        const auto max = maxf.StaticCast<int>();

        for(int y=min.y; y<=max.y; y+=1)
        {
            for(int x=min.x; x<=max.x; x+=1)
            {
                const bool valid_x = IsWithinInclusivei
                (
                    0, x, image->GetWidth() - 1
                );
                const bool valid_y = IsWithinInclusivei
                (
                    0, y, image->GetHeight() - 1
                );
                if(valid_x && valid_y)
                {
                    const auto inside_triangle = IsPointInTriangle
                    (
                        a,
                        b,
                        c,
                        vec2f{static_cast<float>(x), static_cast<float>(y)}
                    );
                    if(inside_triangle)
                    {
                        image->SetPixel(x, y, color);
                    }
                }
            }
        }
    }


    void
    DrawArrow
    (
        Image* image,
        const vec2f& from,
        const vec2f& to,
        const rgbai& color,
        float size
    )
    {
        // based on code from https://www.codeproject.com/Questions/125049/Draw-an-arrow-with-big-cap
        // todo(Gustav): this is too complicated, and hard to customize, different pointy arrows that ain't 90 degrees
        // there must be a better way to do it
        // also generalize it so we can have arrows in dvg/dummper code too
        const vec2f arrowPoint = to;

        const auto arrowLength = Sqrt(Square(Abs(from.x - to.x)) +
                                Square(Abs(from.y - to.y)));

        const auto arrowAngle = atan2(Abs(from.y - to.y),Abs(from.x - to.x));
        const auto angleB = atan2((3 * size), (arrowLength - (3 * size)));
        const auto secondaryLength = (3 * size)/sin(angleB);

        auto angleC = angle::from_degrees(90) - arrowAngle - angleB;
        const auto arrow_point_left_x = from.x > to.x
            ? from.x - (sin(angleC) * secondaryLength)
            : (sin(angleC) * secondaryLength) + from.x
            ;
        const auto arrow_point_left_y = from.y > to.y
            ? from.y - (cos(angleC) * secondaryLength)
            : (cos(angleC) * secondaryLength) + from.y
            ;
        const auto arrowPointLeft = vec2f
        {
            arrow_point_left_x,
            arrow_point_left_y
        };

        //move to the right point
        angleC = arrowAngle - angleB;

        const auto arrow_point_right_x = from.x > to.x
            ? from.x - (cos(angleC) * secondaryLength)
            : (cos(angleC) * secondaryLength) + from.x
            ;
        const auto arrow_point_right_y = from.y > to.y
            ? from.y - (sin(angleC) * secondaryLength)
            : (sin(angleC) * secondaryLength) + from.y
            ;
        const auto arrowPointRight = vec2f
        {
            arrow_point_right_x,
            arrow_point_right_y
        };

        // line
        DrawLineAntialiased(image, crgb(color), from, to);

        // wings
        // DrawLineAntialiased(image, rgb(color), arrowPoint, arrowPointLeft);
        // DrawLineAntialiased(image, rgb(color), arrowPoint, arrowPointRight);
        // DrawLineAntialiased(image, rgb(color), arrowPointLeft, arrowPointRight);

        FillTriangle(image, arrowPointLeft, arrowPoint, arrowPointRight, color);
    }


}  // namespace euphoria::core

