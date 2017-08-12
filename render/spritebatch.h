#ifndef RENDER_SPRITEBATCH_H
#define RENDER_SPRITEBATCH_H

#include <vector>
#include "core/rect.h"
#include "core/size.h"
#include "core/vec2.h"
#include "core/angle.h"
#include "core/rgb.h"

// texture & shader will be handled by 2d world and sprite
class SpriteBatch
{
 public:
  SpriteBatch();
  ~SpriteBatch();

  void
  Begin();
  void
  Quad(const vec2f& pos, const Sizef& quad, const Rectf& uv,
       const vec2f& center, const Angle& rotation, const Rgba& color);
  void
  End();
  void
  Flush();

 private:
  bool               inside_;
  int                count_;
  unsigned int       rendercalls_;
  std::vector<float> data_;
  std::vector<int>   index_;
};

#endif  // RENDER_SPRITEBATCH_H
