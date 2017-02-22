#include "render/scalablesprite.h"

#include "core/assert.h"
#include <iostream>
#include <algorithm>

#include "render/buffer.h"
#include "render/bufferbuilder2d.h"
#include "core/rect.h"
#include "render/texture.h"
#include "render/texturecache.h"

#include "scalingsprite.pb.h"
#include "core/proto.h"

#include "core/tablelayout.h"

////////////////////////////////////////////////////////////////////////////////

namespace {
float CopyData(std::vector<float>* dest, const google::protobuf::RepeatedField<int>& src) {
  dest->reserve(src.size());
  float size = 0;
  for(const int s: src) {
    const float f = static_cast<float>(s);
    dest->push_back(f);
    size += Abs(f);
  }

  if( dest->empty() ) {
    dest->push_back(-100.0f);
    return 100.0f;
  }
  else {
    return size;
  }
}
}

ScalableSprite::ScalableSprite(const std::string& path, const Sizef& size, TextureCache* cache) : texture_(cache->GetTexture(path)), size_(size), dirty_(true) {
  scalingsprite::ScalingSprite sprite;
  LoadProtoText(&sprite, path+".txt");

  max_row_ = CopyData(&rows_, sprite.rows());
  max_col_ = CopyData(&cols_, sprite.cols());
}

ScalableSprite::~ScalableSprite() {
}

void ScalableSprite::SetSize(const Sizef &new_size) {
  assert(this);
  size_ = new_size;
  dirty_ = true;
}

const Sizef ScalableSprite::GetSize() const {
  assert(this);
  return size_;
}

namespace {
  float GetConstantSize(const std::vector<float>& data) {
    float r = 0;
    for(float f: data) {
      if( f > 0 ) {
        r += f;
      }
    }
    return r;
  }
}

const Sizef ScalableSprite::GetMinimumSize() const {
  return Sizef::FromWidthHeight(GetConstantSize(cols_), GetConstantSize(rows_));
}

const Texture2d* ScalableSprite::texture_ptr() const {
  assert(this);
  return texture_;
}

const Ebo* ScalableSprite::ebo_ptr() const {
  assert(this);
  BuildData();
  assert(dirty_ == false);
  assert(ebo_.get() != nullptr);
  return ebo_.get();
}

void ScalableSprite::BuildData() const {
  if( dirty_ == false) return;
  const auto position_cols = PerformTableLayout(cols_, size_.GetWidth());
  const auto position_rows = PerformTableLayout(rows_, size_.GetHeight());

  const auto cols_size = cols_.size();
  const auto rows_size = rows_.size();

  assert(position_rows.size() == rows_size);
  assert(position_cols.size() == cols_size);

  BufferBuilder2d data;
  float position_current_col = 0;
  float uv_current_col = 0;
  for(unsigned int c=0; c<cols_size; ++c) {
    float position_current_row = 0;
    float uv_current_row = 0;

    const auto position_next_col = position_current_col + position_cols[c];
    const auto uv_next_col = uv_current_col + Abs(cols_[c])/max_col_;

    for(unsigned int r=0; r<rows_size; ++r) {
      const auto position_next_row = position_current_row + position_rows[r];
      const auto uv_next_row = uv_current_row + Abs(rows_[r]) / max_row_;

      /*
       current/new + col/row

       cc                      nc
       cr                      cr
       /------------------------\
       | 0                    1 |
       |                        |
       |                        |
       |                        |
       |                        |
       | 3                    2 |
       \------------------------/
       cc                       nc
       nr                       nr

      */

#define MAKE_POINT(COL, ROW) vec2f(position_ ## COL, position_ ## ROW), vec2f(uv_ ## COL, uv_ ## ROW)
      const Point a (MAKE_POINT(current_col, current_row));
      const Point b (MAKE_POINT(next_col, current_row));
      const Point c (MAKE_POINT(current_col, next_row));
      const Point d (MAKE_POINT(next_col, next_row));
      data.AddQuad(a, b, c, d);
#undef MAKE_POINT

      position_current_row = position_next_row;
      uv_current_row = uv_next_row;
    }
    position_current_col = position_next_col;
    uv_current_col = uv_next_col;
  }

  dirty_ = false;
  vao_.reset(new Vao());
  ebo_.reset(new Ebo());
  data.SetupVao(vao_.get());
  data.SetupEbo(ebo_.get());
}

