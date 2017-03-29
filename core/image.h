#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include <string>
#include <vector>

class Image {
 public:
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
