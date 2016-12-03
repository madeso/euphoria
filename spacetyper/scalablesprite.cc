#include "spacetyper/scalablesprite.h"

#include <cassert>
#include <iostream>
#include <algorithm>

#include "spacetyper/vao.h"
#include "spacetyper/rect.h"
#include "spacetyper/texture.h"
#include "spacetyper/texturecache.h"

#include "scalingsprite.pb.h"
#include "spacetyper/proto.h"

////////////////////////////////////////////////////////////////////////////////

class ScalableSpriteData {
 public:
  ScalableSpriteData(const VaoBuilder& avao, const glm::vec2& aworld_size, const glm::vec2& aclient_size, const glm::vec2& aclient_offset)
      : vao(avao), world_size(aworld_size), client_size(aclient_size), client_offset(aclient_offset) {}
  Vao vao;
  glm::vec2 world_size;
  glm::vec2 client_size;
  glm::vec2 client_offset;
};

////////////////////////////////////////////////////////////////////////////////

ScalableSprite::ScalableSprite(Texture2d* texture, const glm::vec2& min_size, SizeType size_type, const glm::vec2& size)
  : texture_(texture)
  , min_size_(min_size)
  , size_type_(size_type)
  , size_(size)
{
}

ScalableSprite::~ScalableSprite() {
}

void ScalableSprite::SetClientSize(const glm::vec2 &new_size) {
  assert(this);
  SetSizeSub(new_size, SizeType::CLIENT);
}

const glm::vec2 ScalableSprite::GetClientSize() const {
  assert(this);
  if( size_type_ == SizeType::CLIENT) return size_;
  PrepareData();
  return data_->client_size;
}

const glm::vec2 ScalableSprite::GetClientOffset() const {
  assert(this);
  PrepareData();
  return data_->client_offset;
}

void ScalableSprite::SetSize(const glm::vec2 &new_size) {
  assert(this);
  SetSizeSub(new_size, SizeType::WORLD);
}

const glm::vec2 ScalableSprite::GetSize() const {
  assert(this);
  if( size_type_ == SizeType::WORLD) return size_;
  PrepareData();
  return data_->world_size;
}

const glm::vec2 ScalableSprite::GetMinimumSize() const {
  assert(this);
  return min_size_;
}

const glm::vec2 ScalableSprite::GetMinimumSize(const glm::vec2& extra) const {
  assert(this);
  return glm::vec2(min_size_.x + extra.x, min_size_.y + extra.y);
}

const Texture2d* ScalableSprite::texture_ptr() const {
  assert(this);
  return texture_;
}

const Vao* ScalableSprite::vao_ptr() const {
  assert(this);
  PrepareData();
  return &data_->vao;
}

std::pair<ScalableSprite::SizeType, glm::vec2> ScalableSprite::GetSizeData() const {
  return std::pair<SizeType, glm::vec2>(size_type_, size_);
};

void ScalableSprite::PrepareData() const {
  assert(this);
  if( data_.get() == nullptr) {
    data_.reset(BuildData());
  }
}

void ScalableSprite::SetSizeSub(const glm::vec2& new_size, SizeType type) {
  assert(this);
  if( data_.get() != nullptr && size_type_ == type && size_ == new_size) {
    // if we have built the data and the size & type is the same
    // then there's no need to rebuild it
    return;
  }
  size_ = new_size;
  size_type_ = type;
  data_.reset();
}

////////////////////////////////////////////////////////////////////////////////

namespace {
  glm::vec2 DetermineSize(const glm::vec2 real_size, Rect<int> pixel_data) {
    return glm::vec2( real_size.x - pixel_data.GetWidth() , real_size.y - pixel_data.GetHeight());
  }

  VaoBuilder BuildNinepatchVao(const Texture2d* texture_, const Rect<int> draw, const glm::vec2& inner_size_) {
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

    const float border_left = std::max(0, draw.left);
    const float border_right = std::max(0.0f, width - draw.right);
    const float border_top = std::max(0, draw.top);
    const float border_bottom = std::max(0.0f, height - draw.bottom);

    const float x0 = 0.0f;
    const float x1 = border_left;
    const float x2 = x1 + inner_size_.x;
    const float x3 = x2 + border_right;

    const float y0 = 0.0f;
    const float y1 = border_top;
    const float y2 = y1 + inner_size_.y;
    const float y3 = y2 + border_bottom;

    const float u0 = 0.0f;
    const float u1 = 0.0f + border_left / width;
    const float u2 = 1.0f - border_right/width;
    const float u3 = 1.0f;

    const float v0 = 0.0f;
    const float v1 = 0.0f + border_top / height;
    const float v2 = 1.0f - border_bottom / height;
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

    return data;
  }
}

class ScalableSpriteNinepatch : public ScalableSprite {
 public:
  ScalableSpriteNinepatch(Texture2d* texture, SizeType size_type, const glm::vec2& size, const Rect<int>& da, const Rect<int>& ca)
      : ScalableSprite(texture, glm::vec2(ca.GetWidth(), ca.GetHeight()), size_type, size)
      , draw_area_(da), client_area_(ca) {}

  glm::vec2 DetermineRealSize() const {
    auto data = GetSizeData();
    const auto size = data.second;
    switch (data.first) {
      case ScalableSprite::SizeType::CLIENT:
        return glm::vec2(size.x + client_area_.GetWidth(), size.y + client_area_.GetHeight());
      case ScalableSprite::SizeType::WORLD:
        return size;
    }
    assert(false && "unhandled SizeType");
  }

  bool ValidateCalculatedSize(const glm::vec2& real_size, const glm::vec2& client_size) const {
    auto data = GetSizeData();
    const auto size = data.second;
    switch (data.first) {
      case ScalableSprite::SizeType::CLIENT:
        return client_size == size;
      case ScalableSprite::SizeType::WORLD:
        return real_size == size;
    }
    assert(false && "unhandled SizeType");
  }

  ScalableSpriteData* BuildData() const override {
    const glm::vec2 real_size = DetermineRealSize();
    const glm::vec2 draw_size = DetermineSize(real_size, draw_area_);
    const glm::vec2 client_size = DetermineSize(real_size, client_area_);

    assert(ValidateCalculatedSize(real_size, client_size));

    return new ScalableSpriteData(BuildNinepatchVao(texture_ptr(), draw_area_, draw_size), real_size, client_size, glm::vec2(client_area_.left, client_area_.top));
  }

  Rect<int> draw_area_;
  Rect<int> client_area_;
};


////////////////////////////////////////////////////////////////////////////////

Rect<int> Convert(const scalingsprite::Rect& r) {
  return Rect<int>::FromLeftRightTopBottom(r.left(), r.right(), r.top(), r.bottom());
}

std::shared_ptr<ScalableSprite> LoadScalableNinepatch(Texture2d* tex, ScalableSprite::SizeType size_type, glm::vec2 size, const scalingsprite::Ninepatch& data) {
  std::shared_ptr<ScalableSprite> ret ( new ScalableSpriteNinepatch(tex, size_type, size, Convert(data.draw()), Convert(data.client())) );
  return ret;
}

std::shared_ptr<ScalableSprite> LoadScalableSprite(const std::string& path, const glm::vec2& size, TextureCache* cache) {
  Texture2d* tex = cache->GetTexture(path);
  const ScalableSprite::SizeType size_type = ScalableSprite::SizeType::WORLD;

  scalingsprite::ScalingSprite sprite;
  LoadProtoText(&sprite, path+".txt");

  // only ninepatch supported...
  assert(sprite.has_ninepatch());
  return LoadScalableNinepatch(tex, size_type, size, sprite.ninepatch());
}

