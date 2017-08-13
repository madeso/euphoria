#include "render/spritebatch.h"
#include "render/gl.h"

#include "core/assert.h"

// vertex + uv + color
const int QUAD_COUNT = 10;
const int STRIDE     = 2 * 4 + 2 * 4 + 4 * 4;

SpriteBatch::SpriteBatch()
    : inside_(false)
    , count_(0)
    , rendercalls_(0)
{
  data_.reserve(STRIDE * QUAD_COUNT);
  index_.reserve(6 * QUAD_COUNT);
}

SpriteBatch::~SpriteBatch() = default;

void
SpriteBatch::Begin()
{
  Assert(!inside_ && "Already open, missing call to end.");
  rendercalls_ = 0;
}

void
SpriteBatch::Quad(const vec2f& pos, const Sizef& quad, const Rectf& uv,
                  const vec2f& center, const Angle& rotation, const Rgba& color)
{
  Assert(inside_ && "batch need to be open");

  if((count_ + 1) >= QUAD_COUNT)
  {
    Flush();
  }

  // add vertices
  const float w          = quad.GetWidth();
  const float h          = quad.GetHeight();
  const float sy         = -Sin(rotation);
  const float left       = pos.x + (-center.x * w) * sy;
  const float right      = pos.x + (-center.x * w + w) * sy;
  const float top        = pos.y + (-center.y * h) * sy;
  const float bottom     = pos.y + (-center.y * h + h) * sy;
  const vec2f upperLeft  = vec2f(left, top);
  const vec2f upperRight = vec2f(right, top);
  const vec2f lowerLeft  = vec2f(left, bottom);
  const vec2f lowerRight = vec2f(right, bottom);

  data_.push_back(upperLeft.x);
  data_.push_back(upperLeft.y);

  data_.push_back(upperRight.x);
  data_.push_back(upperRight.y);

  data_.push_back(lowerRight.x);
  data_.push_back(lowerRight.y);

  data_.push_back(lowerLeft.x);
  data_.push_back(lowerLeft.y);

  // add uv coordinate
  data_.push_back(uv.left);
  data_.push_back(uv.top);
  data_.push_back(uv.right);
  data_.push_back(uv.top);
  data_.push_back(uv.right);
  data_.push_back(uv.bottom);
  data_.push_back(uv.left);
  data_.push_back(uv.bottom);

  // add color
  for(int i = 0; i < 4; ++i)
  {
    data_.push_back(color.GetRed());
    data_.push_back(color.GetGreen());
    data_.push_back(color.GetBlue());
    data_.push_back(color.GetAlpha());
  }

  // add index
  int start = count_ * 4;

  index_.push_back(start + 0);
  index_.push_back(start + 1);
  index_.push_back(start + 2);

  index_.push_back(start + 0);
  index_.push_back(start + 2);
  index_.push_back(start + 3);

  count_ += 1;
}

void
SpriteBatch::End()
{
  Assert(inside_ && "not open, missing begin.");
  Flush();
  inside_ = false;
}

void
SpriteBatch::Flush()
{
  if(count_ == 0)
  {
    return;
  }
  // todo: build vbo & render vbo
  // remove all items
  count_ = 0;
  data_.resize(0);
  rendercalls_ += 1;
}
