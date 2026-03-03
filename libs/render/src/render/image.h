#pragma once

namespace eu::render
{

struct Image
{
    int width;
    int height;
    std::vector<unsigned char> data;
    
    Image() = default;

    void setup_with_alpha_support(int w, int h)
    {
        width = w;
        height = h;
        data.resize(width * height * 4, 0);
    }

    void make_invalid()
    {
        width = 0;
        height = 0;
        data.clear();
    }

    void set_pixel(int x, int y, int r, unsigned char g, unsigned char b, unsigned char a)
    {
        const int index = (y * width + x) * 4;
        data[index + 0] = r;
        data[index + 1] = g;
        data[index + 2] = b;
        data[index + 3] = a;
    }
};

void paste_image(Image* dst, int dst_x, int dst_y, const Image& src);

}
