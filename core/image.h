#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include <string>
#include <vector>
#include <memory>

#include "core/ints.h"
#include "core/rgb.h"
#include "core/memorychunk.h"
#include "core/filesystem.h"

enum class ImageWriteFormat
{
  PNG,
  BMP,
  TGA,
  JPEG
};

class Image
{
 public:
  void
  MakeInvalid();
  bool
  IsValid() const;

  void
  Setup(int image_width, int image_height, bool alpha, int default_value = 0);

  // 0,0 is lower left
  // x-axis is positive right
  // y-axis is positive up
  void
  SetPixel(int x, int y, const Rgb& color);
  void
  SetPixel(int x, int y, const Rgba& color);
  void
  SetPixel(
      int           x,
      int           y,
      unsigned char r,
      unsigned char g,
      unsigned char b,
      unsigned char a);

  Rgba
  GetPixel(int x, int y) const;

  int
  GetWidth() const;
  int
  GetHeight() const;
  bool
  HasAlpha() const;

  const unsigned char*
  GetPixelData() const;

  std::shared_ptr<MemoryChunk>
  Write(ImageWriteFormat format, int jpeg_quality = 100) const;

 private:
  int
  GetPixelByteSize() const;

  fuint64
  GetPixelIndex(int x, int y) const;

  // todo: replace with a array instead of a vector
  std::vector<unsigned char> components;
  int                        width_;
  int                        height_;
  bool                       has_alpha_;
};

class ImageLoadResult
{
 public:
  Image image;

  std::string error;
};

enum class AlphaLoad
{
  Remove,
  Keep
};

// todo: move image loading to a io library instead
ImageLoadResult
LoadImage(FileSystem* fs, const std::string& path, AlphaLoad alpha);

#endif  // CORE_IMAGE_H
