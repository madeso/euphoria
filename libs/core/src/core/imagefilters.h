#pragma once

#include "core/rgb.h"


namespace euphoria::core
{
    struct image;
    struct rgb;
    struct palette;
    struct angle;

    enum class grayscale
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
    make_grayscale(rgbai color, grayscale grayscale);

    void
    make_grayscale(image* image, grayscale grayscale);

    void
    match_palette(image* image, const palette& palette);

    void
    match_palette_dither(image* image, const palette& palette);

    void
    edge_detection(image* image, float r);

    void
    color_detection(image* image, rgb color, float r);

    void
    change_brightness(image* image, int change);

    void
    change_contrast(image* image, const angle& contrast);

    // todo(Gustav):
    // https://www.processing.org/tutorials/pixels/
    // https://www.codeproject.com/Articles/996192/Some-Cool-Image-Effects
    // https://www.gamedev.net/articles/programming/graphics/an-introduction-to-digital-image-processing-r2007/
    // https://github.com/jamieowen/glsl-blend
    // color controls https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch22.html
}
