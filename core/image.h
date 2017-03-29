#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include <string>
#include <vector>

class ImageLoadResult {
 public:
  std::vector<unsigned char> components;
  int width;
  int height;
  bool has_alpha;

  std::string error;
};

enum class AlphaLoad { Remove, Include };

// todo: move image loading to a io library instead
ImageLoadResult LoadImage(const std::string& path, AlphaLoad alpha);

#endif  // CORE_IMAGE_H
