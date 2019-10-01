#ifndef RENDER_SPRITEBATCH_H
#define RENDER_SPRITEBATCH_H

#include <vector>
#include "core/rect.h"
#include "core/size.h"
#include "core/vec2.h"
#include "core/angle.h"
#include "core/rgb.h"

namespace euphoria::render
{

  // texture & shader will be handled by 2d world and sprite
  class SpriteBatch
  {
  public:
    SpriteBatch();
    ~SpriteBatch();

    void
    Begin();
    void
    Quad(
        const core::vec2f& pos,
        const core::Sizef& quad,
        const core::Rectf& uv,
        const core::scale2f& center,
        const core::Angle& rotation,
        const core::Rgba&  color);
    void
    End();
    void
    Flush();

  private:
    bool               inside_;
    unsigned int       count_;
    unsigned int       rendercalls_;
    std::vector<float> data_;
    std::vector<int>   index_;
  };

}

#endif  // RENDER_SPRITEBATCH_H
