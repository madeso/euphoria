#ifndef SPACETYPER_IMAGE_H
#define SPACETYPER_IMAGE_H

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

ImageLoadResult LoadImage(const std::string& path, AlphaLoad alpha);

#endif  // SPACETYPER_IMAGE_H
