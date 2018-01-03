#include "gui/root.h"
#include "gui/load.h"
#include "gui/skin.h"

Root::Root(const Sizef& size)
    : size_(size)
{
}

Root::~Root()
{
}

bool
Root::Load(
    FileSystem*             fs,
    FontCache*                   font,
    const std::string&      path,
    TextureCache*           cache,
    TextBackgroundRenderer* br)
{
  const bool result =
      ::Load(fs, &state_, font, &container_, path, cache, br, &skins_);
  if(result)
  {
    container_.DoLayout(Rectf::FromWidthHeight(size_));
  }
  return result;
}

void
Root::SetInputMouse(const vec2f& pos, bool down)
{
  state_.mouse      = pos;
  state_.mouse_down = down;
}

void
Root::Step(float dt)
{
  state_.Begin();
  container_.Step(dt);
  state_.End();
}

void
Root::Render(SpriteRenderer* sp) const
{
  container_.Render(sp);
}
