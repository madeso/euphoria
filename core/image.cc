#include "core/image.h"

#include <iostream>

#include "core/assert.h"
#include "core/numeric.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

void Image::MakeInvalid() {
  components.resize(0);
  width_ = 0;
  height_ = 0;
  has_alpha_ = false;

  Assert(IsValid() == false);
}

int Image::GetPixelByteSize() const {
  return has_alpha_ ? 4 : 3;
}

void Image::Setup(int image_width, int image_height, bool alpha, int default_value) {
  Assert(image_width > 0);
  Assert(image_height > 0);

  width_ = image_width;
  height_ = image_height;
  has_alpha_ = alpha;

  components.resize(0); // clear all pixels
  const size_t size = width_ * height_ * GetPixelByteSize();
  if(default_value < 0) {
    components.resize(size);
  }
  else {
    Assert(default_value<=255);
    components.resize(size, static_cast<unsigned char>(default_value));
  }

  Assert(IsValid());
}

unsigned long Image::GetPixelIndex(int x, int y) const {
  Assert(x >= 0 && x<width_);
  Assert(y >= 0 && y<height_);

  return (y * static_cast<unsigned long>(width_) + x) * GetPixelByteSize();
}

namespace // local
{
float ToFloat(unsigned char c) {return c/255.0f;}
unsigned char ToUnsignedChar(float f) {return static_cast<unsigned char>(f*255.0f);}
}

void Image::SetPixel(int x, int y, const Rgb& color) {
  SetPixel(x, y, Rgba{color});
}

void Image::SetPixel(int x, int y, const Rgba& color) {
  SetPixel(x,y,
           ToUnsignedChar(color.GetRed()),
           ToUnsignedChar(color.GetGreen()),
           ToUnsignedChar(color.GetBlue()),
           ToUnsignedChar(color.GetAlpha())
  );
}

void Image::SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  Assert(IsWithinInclusivei(0, x, GetWidth()-1));
  Assert(IsWithinInclusivei(0, y, GetHeight()-1));

  const unsigned long base_index = GetPixelIndex(x, y);
  components[base_index + 0] = r;
  components[base_index + 1] = g;
  components[base_index + 2] = b;

  if (has_alpha_) {
    components[base_index + 3] = a;
  }
}

Rgba Image::GetPixel(int x, int y) const {
  Assert(IsWithinInclusivei(0, x, GetWidth()-1));
  Assert(IsWithinInclusivei(0, y, GetHeight()-1));

  const unsigned long base_index = GetPixelIndex(x, y);

  const float red = ToFloat(components[base_index + 0]);
  const float green = ToFloat(components[base_index + 1]);
  const float blue = ToFloat(components[base_index + 2]);

  const float alpha = has_alpha_
                      ? ToFloat(components[base_index + 3])
                      : 1.0f;

  return Rgba{red, green, blue, alpha};
}

bool Image::IsValid() const {
  return width_ > 0 && height_ > 0;
}

int Image::GetWidth() const {
  return width_;
}

int Image::GetHeight() const {
  return height_;
}

bool Image::HasAlpha() const {
  return has_alpha_;
}

const unsigned char* Image::GetPixelData() const {
  return &components[0];
}

namespace // local
{
void DetermineImageSize(void *context, void *data, int size)
{
  Assert(size >= 0);
  auto* total_size = static_cast<unsigned long*>(context);
  *total_size += size;
}

void WriteToMemoryChunkFile(void *context, void *data, int size)
{
  Assert(size >= 0);
  auto* file = static_cast<MemoryChunkFile*>(context);
  file->Write(data, size);
}
}

int WriteImageData(stbi_write_func *func, void *context, int w, int h, int comp, const void  *data, ImageWriteFormat format, int jpeg_quality)
{
  switch(format) {
    case ImageWriteFormat::PNG:
      return stbi_write_png_to_func(func, context, w, h, comp, data, 0);
    case ImageWriteFormat::BMP:
      return stbi_write_bmp_to_func(func, context, w, h, comp, data);
    case ImageWriteFormat::TGA:
      return stbi_write_tga_to_func(func, context, w, h, comp, data);
    case ImageWriteFormat::JPEG:
      return stbi_write_jpg_to_func(func, context, w, h, comp, data, jpeg_quality);
    default:
      Assert(false && "Unhandled case");
      return 0;
  }
}

std::shared_ptr<MemoryChunk> Image::Write(ImageWriteFormat format, int jpeg_quality) const
{
  unsigned long size = 0;
  const int comp = has_alpha_ ? 4 : 3;
  int size_result = WriteImageData(DetermineImageSize, &size, GetWidth(), GetHeight(), comp, GetPixelData(), format, jpeg_quality);
  if(size_result == 0)
  {
    return MemoryChunk::Null();
  }

  Assert(size > 0);
  MemoryChunkFile file {MemoryChunk::Alloc(size)};
  int write_result = WriteImageData(WriteToMemoryChunkFile, &file, GetWidth(), GetHeight(), comp, GetPixelData(), format, jpeg_quality);
  if(write_result == 0)
  {
    return MemoryChunk::Null();
  }

  return file.data;
}

namespace {
unsigned char Select(int ch, unsigned char a, unsigned char b, unsigned char c,
                     unsigned char d) {
  switch (ch) {
    case 1: // grey
      return a;
    case 2: // grey, alpha
      return b;
    case 3: // red, green, blue
      return c;
    case 4: // red, green, blue, alpha
      return d;
    default:
      Assert(false && "unhandled Select channel");
      return 0;
  }
}
}

ImageLoadResult LoadImage(FileSystem* fs, const std::string& path, AlphaLoad alpha) {
  int channels = 0;
  int image_width = 0;
  int image_height = 0;

  auto file_memory = fs->ReadFile(path);
  if (file_memory == nullptr) {
    ImageLoadResult result;
    result.error = "File doesnt exist";
    result.image.MakeInvalid();
    std::cerr << "Failed to open " << path << ": File doesnt exist.\n";
    return result;
  }

  unsigned char* data = stbi_load_from_memory(file_memory->GetData(), file_memory->GetSize(),
                                              &image_width, &image_height, &channels, 0);
  // unsigned char* data = stbi_load(path.c_str(), &image_width, &image_height, &channels, 0);

  if (data == nullptr) {
    ImageLoadResult result;
    result.error = stbi_failure_reason();
    result.image.MakeInvalid();
    std::cerr << "Failed to read " << path << ": " << result.error << "\n";
    return result;
  }

  bool has_alpha = false;
  if (alpha == AlphaLoad::Include) {
    has_alpha = channels == 2 || channels == 4;
  }

  std::cout << "Image: " << path << " " << image_width << "x" << image_height
            << " alpha " << has_alpha << " channels " << channels
            << ".\n";

  ImageLoadResult result;
  result.image.Setup(image_width, image_height, has_alpha, -1);

  for (int y = 0; y < image_height; ++y) {
    for (int x = 0; x < image_width; ++x) {
      const unsigned long src_index = (y * static_cast<unsigned long>(image_width) + x) * channels;

      // get component values
      const unsigned char zero = 0;
      const unsigned char c1 = data[src_index + 0];
      const unsigned char c2 = (channels <= 1) ? zero : data[src_index + 1];
      const unsigned char c3 = (channels <= 2) ? zero : data[src_index + 2];
      const unsigned char c4 = (channels <= 3) ? zero : data[src_index + 3];

      // Gray, Gray+alpha, RGB, RGB+alpha:     gr   gra  rgb  rgba
      const unsigned char r = c1; //           c1   c1   c1    c1
      const unsigned char g = Select(channels, c1,  c1,  c2,   c2);
      const unsigned char b = Select(channels, c1,  c1,  c3,   c3);
      const unsigned char a = Select(channels, 255, c2,  255,  c4);

      result.image.SetPixel(x, y, r, g, b, a);
    }
  }

  stbi_image_free(data);
  return result;
}
