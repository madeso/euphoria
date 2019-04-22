#ifndef CORE_IMAGEOPS_H
#define CORE_IMAGEOPS_H

#include "core/table.h"
#include "core/rgb.h"

class Image;

struct ImageMapAction
{
  char to;
  Rgbi from_color;
};

Table<char> ImageToStringTable(const Image& img, const std::vector<ImageMapAction>& map);

Table<char> ImageToStringTable(const Image& img, bool shorter);

enum class Grayscale
{
  R, G, B,
  Max, Gamma, Linear,
  Average
};
void MakeGrayscale(Image* image, Grayscale grayscale);

// todo:
// https://www.processing.org/tutorials/pixels/
// https://www.codeproject.com/Articles/996192/Some-Cool-Image-Effects
// https://www.gamedev.net/articles/programming/graphics/an-introduction-to-digital-image-processing-r2007/

#endif  // CORE_IMAGEOPS_H
