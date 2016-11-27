#ifndef SPACETYPER_NINEPATCH_H
#define SPACETYPER_NINEPATCH_H

#include <memory>
#include "glm/vec2.hpp"

class Vao;
class Texture2d;

class Ninepatch {
 public:
  Ninepatch(Texture2d* tex, int left, int right, int top, int bottom, const glm::vec2& inner_size);
  ~Ninepatch();

  void SetInnerSize(const glm::vec2& new_size);

  float width() const;
  float height() const;

  const Texture2d* texture_ptr() const;
  const Vao* vao_ptr() const;

 private:
  void BuildVao();

  Texture2d* texture_;
  int left_;
  int right_;
  int top_;
  int bottom_;
  glm::vec2 inner_size_;
  std::unique_ptr<Vao> vao_;
};

#endif  // SPACETYPER_NINEPATCH_H
