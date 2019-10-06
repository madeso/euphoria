#ifndef CORE_IMAGEFILTERS_H
#define CORE_IMAGEFILTERS_H

#include "core/rgb.h"
#include "core/enumtostring.h"

namespace euphoria::core
{
    class Image;
    class Rgb;
    struct Palette;

    enum class Grayscale
    {
        R,
        G,
        B,
        Max,
        Gamma,
        Linear,
        Average
    };

    struct ImageFilter
    {
        Image* image;

        void
        MakeGrayscale(Grayscale grayscale);
        void
        MatchPalette(const Palette& palette);
        void
        MatchPaletteDither(const Palette& palette);
        void
        EdgeDetection(float r);
        void
        ColorDetection(Rgb color, float r);
        void
        ChangeBrightness(int change);
        void
        ChangeContrast(float contrast);

        // todo:
        // https://www.processing.org/tutorials/pixels/
        // https://www.codeproject.com/Articles/996192/Some-Cool-Image-Effects
        // https://www.gamedev.net/articles/programming/graphics/an-introduction-to-digital-image-processing-r2007/
        // https://github.com/jamieowen/glsl-blend
        // color controls https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch22.html
    };
}  // namespace euphoria::core

DECLARE_ENUM_LIST(euphoria::core::Grayscale)

#endif  // CORE_IMAGEFILTERS_H
