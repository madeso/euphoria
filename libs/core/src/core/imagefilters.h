#pragma once

#include "core/rgb.h"


namespace euphoria::core
{
    struct Image;
    struct rgb;
    struct Palette;
    struct angle;

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

    rgbai
    make_grayscale(rgbai color, Grayscale grayscale);

    void
    make_grayscale(Image* image, Grayscale grayscale);

    void
    match_palette(Image* image, const Palette& palette);

    void
    match_palette_dither(Image* image, const Palette& palette);

    void
    edge_detection(Image* image, float r);

    void
    color_detection(Image* image, rgb color, float r);

    void
    change_brightness(Image* image, int change);

    void
    change_contrast(Image* image, const angle& contrast);

    // todo(Gustav):
    // https://www.processing.org/tutorials/pixels/
    // https://www.codeproject.com/Articles/996192/Some-Cool-Image-Effects
    // https://www.gamedev.net/articles/programming/graphics/an-introduction-to-digital-image-processing-r2007/
    // https://github.com/jamieowen/glsl-blend
    // color controls https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch22.html
}
