#include "core/image.h"

#include "core/assert.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

void Image::Clear() {
  Assert(this);
  components.resize(0);
  width = height = 0;
  has_alpha = false;

  Assert(IsValid() == false);
}

int Image::GetPixelByteSize() const {
  Assert(this);
  return has_alpha ? 4 : 3;
}

void Image::Setup(int image_width, int image_height, bool alpha) {
  Assert(this);
  Assert(image_width > 0);
  Assert(image_height > 0);

  width = image_width;
  height = image_height;
  has_alpha = alpha;

  components.resize(width * height * GetPixelByteSize(), 0);

  Assert(IsValid());
}

unsigned long Image::GetPixelIndex(int x, int y) const {
  Assert(this);
  Assert(x >= 0 && x<width);
  Assert(y >= 0 && y<height);

  return (y * width + x) * GetPixelByteSize();
}

void Image::SetPixel(int x, int y, unsigned char r, unsigned char g,
                     unsigned char b, unsigned char a) {
  Assert(this);

  const unsigned long dest = GetPixelIndex(x, y);
  components[dest + 0] = r;
  components[dest + 1] = g;
  components[dest + 2] = b;

  if (has_alpha) {
    components[dest + 3] = a;
  }
}

bool Image::IsValid() const {
  Assert(this);
  return width > 0 && height > 0;
}

int Image::GetWidth() const {
  Assert(this);
  return width;
}

int Image::GetHeight() const {
  Assert(this);
  return height;
}

bool Image::HasAlpha() const {
  Assert(this);
  return has_alpha;
}

const unsigned char* Image::GetPixelData() const {
  Assert(this);
  return &components[0];
}

unsigned char Select(int ch, unsigned char a, unsigned char b, unsigned char c,
                     unsigned char d) {
  switch (ch) {
    case 1:
      return a;
    case 2:
      return b;
    case 3:
      return c;
    case 4:
      return d;
    default:
      Assert(false && "unhandled Select channel");
      return 0;
  }
}

ImageLoadResult LoadImage(const std::string& path, AlphaLoad alpha) {
  ImageLoadResult result;

  int channels = 0;
  int image_width = 0;
  int image_height = 0;
  unsigned char* data = stbi_load(path.c_str(), &image_width, &image_height, &channels, 0);

  if (data == NULL) {
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

  // const unsigned char dest_c = result.image.has_alpha ? 4 : 3;
  // result.image.components.resize(result.image.width * result.image.height * dest_c, 0);
  result.image.Setup(image_width, image_height, has_alpha);

  for (int y = 0; y < image_height; ++y) {
    for (int x = 0; x < image_width; ++x) {
      const unsigned long src = (y * image_width + x) * channels;

      const unsigned char c1 = data[src + 0];
      const unsigned char c2 = (channels <= 1 ? 0 : data[src + 1]);
      const unsigned char c3 = (channels <= 2 ? 0 : data[src + 2]);
      const unsigned char c4 = (channels <= 3 ? 0 : data[src + 3]);

      const unsigned char r = c1;
      const unsigned char g = Select(channels, c1, c1, c2, c2);
      const unsigned char b = Select(channels, c1, c1, c3, c3);
      const unsigned char a = Select(channels, 255, c2, 255, c4);

      result.image.SetPixel(x, y, r, g, b, a);
    }
  }

  stbi_image_free(data);
  return result;
}
