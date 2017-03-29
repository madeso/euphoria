#include "core/image.h"

#include "core/assert.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

void Image::Clear() {
  Assert(this);
  components.resize(0);
  width_ = 0;
  height_ = 0;
  has_alpha_ = false;

  Assert(IsValid() == false);
}

int Image::GetPixelByteSize() const {
  Assert(this);
  return has_alpha_ ? 4 : 3;
}

void Image::Setup(int image_width, int image_height, bool alpha, int default_value) {
  Assert(this);
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
  Assert(this);
  Assert(x >= 0 && x<width_);
  Assert(y >= 0 && y<height_);

  return (y * width_ + x) * GetPixelByteSize();
}

void Image::SetPixel(int x, int y, unsigned char r, unsigned char g,
                     unsigned char b, unsigned char a) {
  Assert(this);

  const unsigned long base_index = GetPixelIndex(x, y);
  components[base_index + 0] = r;
  components[base_index + 1] = g;
  components[base_index + 2] = b;

  if (has_alpha_) {
    components[base_index + 3] = a;
  }
}

bool Image::IsValid() const {
  Assert(this);
  return width_ > 0 && height_ > 0;
}

int Image::GetWidth() const {
  Assert(this);
  return width_;
}

int Image::GetHeight() const {
  Assert(this);
  return height_;
}

bool Image::HasAlpha() const {
  Assert(this);
  return has_alpha_;
}

const unsigned char* Image::GetPixelData() const {
  Assert(this);
  return &components[0];
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

ImageLoadResult LoadImage(const std::string& path, AlphaLoad alpha) {
  int channels = 0;
  int image_width = 0;
  int image_height = 0;
  unsigned char* data = stbi_load(path.c_str(), &image_width, &image_height, &channels, 0);

  if (data == NULL) {
    ImageLoadResult result;
    result.error = stbi_failure_reason();
    result.image.Clear();
    std::cerr << "Failed to load " << path << ": " << result.error << "\n";
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
      const unsigned long src_index = (y * image_width + x) * channels;

      // get component values
      const unsigned char c1 = data[src_index + 0];
      const unsigned char c2 = (channels <= 1 ? 0 : data[src_index + 1]);
      const unsigned char c3 = (channels <= 2 ? 0 : data[src_index + 2]);
      const unsigned char c4 = (channels <= 3 ? 0 : data[src_index + 3]);

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
