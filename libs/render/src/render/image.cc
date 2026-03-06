#include "render/image.h"

#include "assert/assert.h"

namespace eu::render
{



Image::Image() = default;

bool Image::is_valid() const
{
    return width > 0 && height > 0 && data.size() == static_cast<size_t>(width * height * 4);
}



void Image::setup_with_alpha_support(int w, int h)
{
    width = w;
    height = h;
    data.resize(width * height * 4, 0);
}



void Image::make_invalid()
{
    width = 0;
    height = 0;
    data.clear();
}



void Image::set_pixel(int x, int y, int r, unsigned char g, unsigned char b, unsigned char a)
{
    const int index = (y * width + x) * 4;
    data[index + 0] = r;
    data[index + 1] = g;
    data[index + 2] = b;
    data[index + 3] = a;
}

void Image::set_pixel(int x, int y, const PixelColor& color)
{
    set_pixel(x, y, color.r, color.g, color.b, color.a);
}


PixelColor Image::get_pixel(int x, int y) const
{
    ASSERTX(x >= 0 && x < width && y >= 0 && y < height, x, y, width, height);
    const auto index = (y * width + x) * 4;
    return {
        .r = data[index + 0],
        .g = data[index + 1],
        .b = data[index + 2],
        .a = data[index + 3]
    };
}



void paste_image(Image* dst, int dst_x, int dst_y, const Image& src)
{
    // Assert valid images
    ASSERT(dst != nullptr);
    ASSERT(dst->is_valid());
    ASSERT(src.is_valid());

    for (int local_y = 0; local_y < src.height; local_y += 1)
    {
        const auto y = dst_y + local_y;
        if (y < 0 || y >= dst->height)
        {
            // if y is outside the destination image, skip the entire horizontal row
            continue;
        }

        for (int local_x = 0; local_x < src.width; local_x += 1)
        {
            const auto x = dst_x + local_x;
            if (x < 0 || x >= dst->width)
            {
                // if x is outside the destination image, skip this pixel
                continue;
            }

            const auto src_color = src.get_pixel(local_x, local_y);
            dst->set_pixel(x, y, src_color);
        }
    }
}


}
