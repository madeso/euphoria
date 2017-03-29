#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include <string>
#include <vector>

class Image {
 public:
  void Clear();
  void Setup(int image_width, int image_height, bool alpha);
  void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
  bool IsValid() const;

  int GetWidth() const;
  int GetHeight() const;
  bool HasAlpha() const;
  const unsigned char* GetPixelData() const;

 private:
  std::vector<unsigned char> components;
  int width;
  int height;
  bool has_alpha;
};

class ImageLoadResult {
 public:
  Image image;

  std::string error;
};

enum class AlphaLoad { Remove, Include };

// todo: move image loading to a io library instead
ImageLoadResult LoadImage(const std::string& path, AlphaLoad alpha);

#endif  // CORE_IMAGE_H
