#include "core/canvaslogic.h"
#include "core/numeric.h"

void
CanvasLogic::Pan(const vec2f& p)
{
  scroll += p;
}

void
CanvasLogic::Zoom(const point2f& mouse, float zoom)
{
  // todo: change to use ScreenToWorld
  const auto focus = Scale(mouse - scroll, 1 / scale);

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
  const auto new_focus = scroll + Scale(focus, scale);
  scroll               = scroll + vec2f::FromTo(new_focus, mouse);
}

point2f
CanvasLogic::WorldToScreen(const point2f& p) const
{
  return scroll + Scale(p, scale);
}

point2f
CanvasLogic::ScreenToWorld(const point2f& p) const
{
  return Scale(p - scroll, 1 / scale);
}
