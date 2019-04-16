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

#endif  // CORE_IMAGEOPS_H
