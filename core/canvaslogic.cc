#include "core/canvaslogic.h"
#include "core/numeric.h"

namespace euphoria::core
{

void
CanvasLogic::Pan(const vec2f& p)
{
  scroll += p;
}

void
CanvasLogic::Zoom(const vec2f& mouse, float zoom)
{
  // todo: change to use ScreenToWorld
  const auto focus = (mouse - scroll)/ scale;

  const float scale_factor = 1 + 0.01f * Abs(zoom);

  if(zoom < 0.0f)
  {
    scale /= scale_factor;
  }

  if(zoom > 0.0f)
  {
    scale *= scale_factor;
  }

  scale = KeepWithin(scale_range, scale);

  // todo: change to use WorldToScreen
  const auto new_focus = scroll + focus*scale;
  scroll               = scroll + vec2f::FromTo(new_focus, mouse);
}

vec2f
CanvasLogic::WorldToScreen(const vec2f& p) const
{
  return scroll + p*scale;
}

vec2f
CanvasLogic::ScreenToWorld(const vec2f& p) const
{
  return (p - scroll)/ scale;
}
}
