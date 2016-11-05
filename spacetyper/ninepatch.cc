#include "spacetyper/ninepatch.h"

#include "spacetyper/vao.h"
#include "spacetyper/texture.h"

Ninepatch::Ninepatch(Texture2d* tex, int left, int right, int top, int bottom, const glm::vec2& inner_size)
: texture_(tex), left_(left), right_(right), top_(top), bottom_(bottom), inner_size_(inner_size) {
  BuildVao();
}

Ninepatch::~Ninepatch() {
  vao_.reset();
}

void Ninepatch::SetInnerSize(const glm::vec2& new_size) {
  inner_size_ = new_size;
  BuildVao();
}

void Ninepatch::BuildVao() {
  VaoBuilder data;

  // 00     10                20     30
  // +------+-----------------+------+
  // |      |                 |      |
  // |01    |11               |21    | 31
  // +------+-----------------+------+
  // |      |                 |      |
  // |      |                 |      |
  // |      |                 |      |
  // |      |                 |      |
  // |      |                 |      |
  // |      |                 |      |
  // |02    |12               |22    | 32
  // +------+-----------------+------+
  // |      |                 |      |
  // |03    |13               |23    | 33
  // +------+-----------------+------+
  //

  const float width = texture_->width();
  const float height = texture_->height();

  const float x0 = 0.0f;
  const float x1 = left_;
  const float x2 = x1 + inner_size_.x;
  const float x3 = x2 + right_;

  const float y0 = 0.0f;
  const float y1 = top_;
  const float y2 = y1 + inner_size_.y;
  const float y3 = y2 + bottom_;

  const float u0 = 0.0f;
  const float u1 = 0.0f + left_ / width;
  const float u2 = 1.0f - right_/width;
  const float u3 = 1.0f;

  const float v0 = 0.0f;
  const float v1 = 0.0f + top_ / height;
  const float v2 = 1.0f - bottom_ / height;
  const float v3 = 1.0f;

#define P(A, B) Point(x ## A, y ## B, u ## A, v ## B)
  data.quad(P(0,0), P(1,0), P(0,1), P(1,1))
      .quad(P(1,0), P(2,0), P(1,1), P(2,1))
      .quad(P(2,0), P(3,0), P(2,1), P(3,1));
  data.quad(P(0,1), P(1,1), P(0,2), P(1,2))
      .quad(P(1,1), P(2,1), P(1,2), P(2,2))
      .quad(P(2,1), P(3,1), P(2,2), P(3,2));
  data.quad(P(0,2), P(1,2), P(0,3), P(1,3))
      .quad(P(1,2), P(2,2), P(1,3), P(2,3))
      .quad(P(2,2), P(3,2), P(2,3), P(3,3));

  // basic scale - like a sprite
  // data.quad(P(0,0), P(3,0), P(0,3), P(3,3));
#undef P

  vao_.reset(new Vao(data) );
}

float Ninepatch::width() const {
  return inner_size_.x + left_ + right_;
}

float Ninepatch::height() const {
  return inner_size_.y + top_ + bottom_;
}

const Texture2d* Ninepatch::texture_ptr() const {
  return texture_;
}

const Vao* Ninepatch::vao_ptr() const {
  return vao_.get();
}
