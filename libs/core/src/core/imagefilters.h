#pragma once

#include "core/rgb.h"


namespace euphoria::core
{
    struct Image;
    struct Rgb;
    struct Palette;
    struct Angle;

    enum class Grayscale
    {
        r,
        g,
        b,
        a,
        max,
        gamma,
        linear,
        average
    };

    Rgbai make_grayscale(Rgbai color, Grayscale grayscale);
    void make_grayscale(Image* image, Grayscale grayscale);
    void match_palette(Image* image, const Palette& palette);
    void match_palette_dither(Image* image, const Palette& palette);
    void filter_edge_detection(Image* image, float r);
    void filter_color_detection(Image* image, Rgb color, float r);
    void change_brightness(Image* image, int change);
    void change_contrast(Image* image, const Angle& contrast);

    // todo(Gustav):
    // https://www.processing.org/tutorials/pixels/
    // https://www.codeproject.com/Articles/996192/Some-Cool-Image-Effects
    // https://www.gamedev.net/articles/programming/graphics/an-introduction-to-digital-image-processing-r2007/
    // https://github.com/jamieowen/glsl-blend
    // color controls https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch22.html
}
