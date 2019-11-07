#include "core/image_draw.h"

#include "core/numeric.h"
#include "core/assert.h"
#include "core/range.h"
#include "core/fonts.h"
#include "core/utf8.h"
#include "core/rgb_blend.h"

#include <utility>


namespace euphoria::core
{
    Recti
    WholeImage(const Image& image)
    {
        return Recti::FromTopLeftWidthHeight(
                image.GetHeight(), 0, image.GetWidth(), image.GetHeight());
    }

    void
    Clear(Image* image, const Rgbi& color)
    {
        ASSERT(image);
        return DrawRect(image, color, WholeImage(*image));
    }

    void
    DrawRect(Image* image, const Rgbi& color, const Recti& rect)
    {
        ASSERT(image);
        const int left   = rect.TopLeft().x;
        const int right  = rect.TopRight().x;
        const int top    = rect.TopLeft().y;
        const int bottom = rect.BottomLeft().y;
        // ASSERTX(left >= 0, left);
        // ASSERTX(bottom >= 0, bottom);
        for(int y = bottom; y < top; ++y)
        {
            if(y < 0 || y >= image->GetHeight())
                continue;
            for(int x = left; x < right; ++x)
            {
                if(x < 0 || x >= image->GetWidth())
                    continue;
                image->SetPixel(x, y, color);
            }
        }
    }


    void
    DrawSquare(Image* image, const Rgbi& color, int x, int y, int size)
    {
        ASSERT(image);
        // is the +1 right?
        DrawRect(
                image,
                color,
                Recti::FromTopLeftWidthHeight(y + 1, x, size, size));
    }

    void
    DrawCircle(
            Image*       image,
            const Rgb&   color,
            const vec2i& center,
            float        radius,
            float        softness,
            float        inner)
    {
        ASSERT(image);
        const int left = Max(0, Floori(center.x - radius - softness));
        const int right
                = Min(image->GetWidth(), Ceili(center.x + radius + softness));
        const int top = Max(0, Floori(center.y - radius - softness));
        const int bottom
                = Min(image->GetHeight(), Ceili(center.y + radius + softness));

        // color modes
        // nothing INNER-SOFTNESS fade INNER full RADIUS fade RADIUS+SOFTNESS nothing

        for(int y = top; y < bottom; ++y)
        {
            for(int x = left; x < right; ++x)
            {
                // todo: use length squared!
                const float sq = vec2f::FromTo(
                                         vec2f {static_cast<float>(x),
                                                static_cast<float>(y)},
                                         center.StaticCast<float>())
                                         .GetLength();
                bool  blend        = false;
                float blend_factor = 1.0f;

                const auto a = MakeRange(inner - softness, inner);
                const auto b = MakeRange(radius, radius + softness);

                if(IsWithin(a, sq))
                {
                    blend_factor = To01(a, sq);
                    blend        = true;
                }
                else if(IsWithin(b, sq))
                {
                    blend_factor = 1.0f - To01(b, sq);
                    blend        = true;
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

                const Rgb paint_color = blend ? RgbTransform::Transform(
                                                rgb(image->GetPixel(x, y)),
                                                blend_factor,
                                                color)
                                              : color;

                image->SetPixel(x, y, Rgbi {paint_color});
            }
        }
    }

    void
    DrawLineFast(
            Image*       image,
            const Rgbi&  color,
            const vec2i& from,
            const vec2i& to)
    {
        ASSERT(image);
        // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

        const int x0 = from.x;
        const int y0 = from.y;
        const int x1 = to.x;
        const int y1 = to.y;

        int         deltax = x1 - x0;
        int         deltay = y1 - y0;
        const float deltaerr
                = deltax == 0 ? -1 : Abs(deltay / static_cast<float>(deltax));
        float error = deltaerr - 0.5f;
        int   y     = y0;
        for(int x = x0; x < x1; x += 1)
        {
            const bool valid_x
                    = IsWithinInclusivei(0, x, image->GetWidth() - 1);
            const bool valid_y
                    = IsWithinInclusivei(0, y, image->GetHeight() - 1);
            if(valid_x && valid_y)
            {
                image->SetPixel(x, y, color);
            }

            if(deltaerr > 0)
            {
                error = error + deltaerr;
                if(error >= 0.5)
                {
                    if(deltay > 0)
                    {
                        y = y + 1;
                    }
                    else
                    {
                        y = y - 1;
                    }
                }
                error = error - 1.0;
            }
        }
    }

    int
    Round(float x)
    {
        return Floori(x + 0.5f);
    }

    // fractional part of x
    float
    GetFractionalPart(float x)
    {
        return x - Floor(x);
    }

    float
    GetOneMinusFractionalPart(float x)
    {
        return 1.0f - GetFractionalPart(x);
    }

    void
    Plot(int x, int y, float brightness, const Rgb& color, Image* image)
    {
        ASSERT(image);

        const bool valid_x = IsWithinInclusivei(0, x, image->GetWidth() - 1);
        const bool valid_y = IsWithinInclusivei(0, y, image->GetHeight() - 1);
        if(valid_x && valid_y)
        {
            const Rgb paint_color = RgbTransform::Transform(
                    rgb(image->GetPixel(x, y)), brightness, color);
            image->SetPixel(x, y, Rgbi {paint_color});
        }
    }

    void
    DrawLineAntialiased(
            Image*       image,
            const Rgb&   color,
            const vec2i& from,
            const vec2i& to)
    {
        ASSERT(image);
        return DrawLineAntialiased(
                image, color, from.StaticCast<float>(), to.StaticCast<float>());
    }

    void
    DrawLineAntialiased(
            Image*       image,
            const Rgb&   color,
            const vec2f& from,
            const vec2f& to)
    {
        ASSERT(image);
        // https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
        float x0 = from.x;
        float y0 = from.y;
        float x1 = to.x;
        float y1 = to.y;


        bool steep = Abs(y1 - y0) > Abs(x1 - x0);

        if(steep)
        {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }
        if(x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        float dx       = x1 - x0;
        float dy       = y1 - y0;
        float gradient = dx == 0.0 ? 1.0 : dy / dx;

        // handle first endpoint
        int   xend  = Round(x0);
        float yend  = y0 + gradient * (xend - x0);
        float xgap  = GetOneMinusFractionalPart(x0 + 0.5);
        int   xpxl1 = xend;  // this will be used in the main loop
        float ypxl1 = Floori(yend);
        if(steep)
        {
            Plot(ypxl1,
                 xpxl1,
                 GetOneMinusFractionalPart(yend) * xgap,
                 color,
                 image);
            Plot(ypxl1 + 1,
                 xpxl1,
                 GetFractionalPart(yend) * xgap,
                 color,
                 image);
        }
        else
        {
            Plot(xpxl1,
                 ypxl1,
                 GetOneMinusFractionalPart(yend) * xgap,
                 color,
                 image);
            Plot(xpxl1,
                 ypxl1 + 1,
                 GetFractionalPart(yend) * xgap,
                 color,
                 image);
        }
        int intery = yend + gradient;  // first y-intersection for the main loop

        // handle second endpoint
        xend      = Round(x1);
        yend      = y1 + gradient * (xend - x1);
        xgap      = GetFractionalPart(x1 + 0.5);
        int xpxl2 = xend;  // this will be used in the main loop
        int ypxl2 = Floori(yend);
        if(steep)
        {
            Plot(ypxl2,
                 xpxl2,
                 GetOneMinusFractionalPart(yend) * xgap,
                 color,
                 image);
            Plot(ypxl2 + 1,
                 xpxl2,
                 GetFractionalPart(yend) * xgap,
                 color,
                 image);
        }
        else
        {
            Plot(xpxl2,
                 ypxl2,
                 GetOneMinusFractionalPart(yend) * xgap,
                 color,
                 image);
            Plot(xpxl2,
                 ypxl2 + 1,
                 GetFractionalPart(yend) * xgap,
                 color,
                 image);
        }

        // main loop
        if(steep)
        {
            for(int x = xpxl1 + 1; x < xpxl2 - 1; x += 1)
            {
                Plot(Floori(intery),
                     x,
                     GetOneMinusFractionalPart(intery),
                     color,
                     image);
                Plot(Floori(intery) + 1,
                     x,
                     GetFractionalPart(intery),
                     color,
                     image);
                intery = intery + gradient;
            }
        }
        else
        {
            for(int x = xpxl1 + 1; x < xpxl2 - 1; x += 1)
            {
                Plot(x,
                     Floori(intery),
                     GetOneMinusFractionalPart(intery),
                     color,
                     image);
                Plot(x,
                     Floori(intery) + 1,
                     GetFractionalPart(intery),
                     color,
                     image);
                intery = intery + gradient;
            }
        }
    }

    Rgba Tint(const Rgba& c, const Rgb& tint)
    {
        return
            {
                {
                    c.r * tint.r,
                    c.g * tint.g,
                    c.b * tint.b
                },
                c.a
            };
    }

    Rgbai Tint(const Rgbai& c, const Rgbi& tint)
    {
        return rgbai(Tint(rgba(c), rgb(tint)));
    }

    void
    SimpleImageBlend(Image* dst, const vec2i& p, const Image& src, const Rgbi& tint)
    {
        for(int y=0; y<src.GetHeight(); y+=1)
        for(int x=0; x<src.GetWidth();  x+=1)
        {
            const auto dx = p.x + x;
            const auto dy = p.y + y;
            if(dx >= dst->GetWidth()) continue;
            if(dy >= dst->GetHeight()) continue;
            const auto dst_color = dst->GetPixel(dx, dy);
            const auto src_color = src.GetPixel(x, y);
            const auto tinted_color = Tint(src_color, tint);
            const auto result_color = Blend(tinted_color, dst_color);
            dst->SetPixel(dx, dy, result_color);
        }
    }

    void
    DrawText(
            Image*             image,
            const vec2i&       start_pos,
            const std::string& text,
            const Rgbi&        color,
            const LoadedFont& font)
    {
        ASSERT(image);

        vec2i pos = start_pos;
        Utf8ToCodepoints(text, [&](unsigned int cp)
        {
            if(cp == '\n')
            {
                pos.x = start_pos.x;
                // todo(Gustav): font needs lineheight...
                pos.y -= 8;
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
    PasteImage(Image* image, const vec2i& position, const Image& source_image)
    {
        ASSERT(image);

        for(int y = 0; y < source_image.GetHeight(); ++y)
        {
            for(int x = 0; x < source_image.GetWidth(); ++x)
            {
                image->SetPixel(
                        position.x + x,
                        position.y + y,
                        source_image.GetPixel(x, y));
            }
        }
    }

}  // namespace euphoria::core
