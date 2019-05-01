#ifndef CORE_IMAGEFILTERS_H
#define CORE_IMAGEFILTERS_H

#include "core/rgb.h"
#include "core/enumtostring.h"

class Image;
class Rgb;
class Palette;

enum class Grayscale
{
  R, G, B,
  Max, Gamma, Linear,
  Average
};
BEGIN_ENUM_LIST(Grayscale)
  ENUM_VALUE(Grayscale, R)
  ENUM_VALUE(Grayscale, G)
  ENUM_VALUE(Grayscale, B)
  ENUM_VALUE(Grayscale, Max)
  ENUM_VALUE(Grayscale, Gamma)
  ENUM_VALUE(Grayscale, Linear)
  ENUM_VALUE(Grayscale, Average)
END_ENUM_LIST()

void MakeGrayscale(Image* image, Grayscale grayscale);

void MatchPalette(Image* image, const Palette& palette);

Image MatchPaletteDither(const Image& image, const Palette& palette);

Image EdgeDetection(const Image& image, float r);

void ColorDetection(Image* image, Rgb color, float r);

void ChangeBrightness(Image* image, int change);

void ChangeContrast(Image* image, float contrast);

// todo:
// https://www.processing.org/tutorials/pixels/
// https://www.codeproject.com/Articles/996192/Some-Cool-Image-Effects
// https://www.gamedev.net/articles/programming/graphics/an-introduction-to-digital-image-processing-r2007/

#endif  // CORE_IMAGEFILTERS_H

