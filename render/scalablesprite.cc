#include "render/scalablesprite.h"

#include "core/assert.h"
#include <iostream>
#include <algorithm>

#include "render/buffer2d.h"
#include "render/bufferbuilder2d.h"
#include "core/rect.h"
#include "render/texture.h"
#include "render/texturecache.h"
#include "render/spriterender.h"

#include "gaf_scalingsprite.h"
#include "core/proto.h"

#include "core/tablelayout.h"

////////////////////////////////////////////////////////////////////////////////

namespace
{
  float
  CopyData(std::vector<float>* dest, const std::vector<int>& src)
  {
    dest->reserve(src.size());
    float size = 0;
    for(const int s : src)
    {
      const auto f = static_cast<float>(s);
      dest->push_back(f);
      size += Abs(f);
    }

    if(dest->empty())
    {
      dest->push_back(-100.0f);
      return 100.0f;
    }

    return size;
  }
}  // namespace

ScalableSprite::ScalableSprite(
    FileSystem*        fs,
    const std::string& path,
    const Sizef&       size,
    TextureCache*      cache)
    : texture_(cache->GetTexture(path))
{
  scalingsprite::ScalingSprite sprite;

  LoadProtoJson(fs, &sprite, path + ".json");

  max_row_ = CopyData(&rows_, sprite.rows);
  max_col_ = CopyData(&cols_, sprite.cols);
}

ScalableSprite::~ScalableSprite() = default;

namespace
{
  float
  GetConstantSize(const std::vector<float>& data)
  {
    float r = 0;
    for(float f : data)
    {
      if(f > 0)
      {
        r += f;
      }
    }
    return r;
  }
}  // namespace

const Sizef
ScalableSprite::GetMinimumSize() const
{
  return Sizef::FromWidthHeight(GetConstantSize(cols_), GetConstantSize(rows_));
}


void
ScalableSprite::Render(
    SpriteRenderer* sr, const Rectf& rect, const Rgba& tint) const
{
  const auto size_         = rect.GetSize();
  const auto pos           = rect.GetBottomLeft();
  const auto position_cols = PerformTableLayout(cols_, size_.GetWidth());
  const auto position_rows = PerformTableLayout(rows_, size_.GetHeight());

  const auto cols_size = cols_.size();
  const auto rows_size = rows_.size();

  ASSERT(position_rows.size() == rows_size);
  ASSERT(position_cols.size() == cols_size);

  float position_current_col = 0;
  float uv_current_col       = 0;
  for(unsigned int c = 0; c < cols_size; ++c)
  {
    float position_current_row = size_.GetHeight();
    float uv_current_row       = 1;

    const auto position_next_col = position_current_col + position_cols[c];
    const auto uv_next_col       = uv_current_col + Abs(cols_[c]) / max_col_;

    for(unsigned int r = 0; r < rows_size; ++r)
    {
      const auto position_next_row = position_current_row - position_rows[r];
      const auto uv_next_row       = uv_current_row - Abs(rows_[r]) / max_row_;

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
      ASSERTX(
          position_current_row > position_next_row,
          position_current_row,
          position_next_row);
      ASSERTX(uv_current_row > uv_next_row, uv_current_row, uv_next_row);
      const auto position_rect = Rectf::FromLeftRightTopBottom(
          position_current_col,
          position_next_col,
          position_current_row,
          position_next_row);
      const auto uv_rect = Rectf::FromLeftRightTopBottom(
          uv_current_col, uv_next_col, uv_current_row, uv_next_row);

      sr->DrawRect(
          *texture_.get(),
          position_rect.OffsetCopy(pos),
          uv_rect,
          Angle::Zero(),
          vec2f{0, 0},
          tint);

      position_current_row = position_next_row;
      uv_current_row       = uv_next_row;
    }
    position_current_col = position_next_col;
    uv_current_col       = uv_next_col;
  }
}
