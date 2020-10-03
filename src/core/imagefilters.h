#ifndef CORE_IMAGEFILTERS_H
#define CORE_IMAGEFILTERS_H

#include "core/rgb.h"
#include "core/enumtostring.h"

namespace euphoria::core
{
    struct Image;
    struct Rgb;
    struct Palette;

    enum class Grayscale
    {
        R,
        G,
        B,
        A,
        Max,
        Gamma,
        Linear,
        Average
    };

    Rgbai
    MakeGrayscale(Rgbai color, Grayscale grayscale);

    void
    MakeGrayscale(Image* image, Grayscale grayscale);

    void
    MatchPalette(Image* image, const Palette& palette);

    void
    MatchPaletteDither(Image* image, const Palette& palette);

    void
    EdgeDetection(Image* image, float r);

    void
    ColorDetection(Image* image, Rgb color, float r);

    void
    ChangeBrightness(Image* image, int change);

    void
    ChangeContrast(Image* image, float contrast);

    // todo:
    // https://www.processing.org/tutorials/pixels/
    // https://www.codeproject.com/Articles/996192/Some-Cool-Image-Effects
    // https://www.gamedev.net/articles/programming/graphics/an-introduction-to-digital-image-processing-r2007/
    // https://github.com/jamieowen/glsl-blend
    // color controls https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch22.html
}  // namespace euphoria::core

#endif  // CORE_IMAGEFILTERS_H