#include "gui/root.h"

#include "render/spriterender.h"

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
    vfs::FileSystem*        fs,
    FontCache*         font,
    const std::string& path,
    TextureCache*      cache)
{
  const bool result = ::Load(this, fs, font, path, cache);
  if(result)
  {
    container_.DoLayout(Rectf::FromWidthHeight(size_));
  }
  return result;
}

void
Root::SetInputMouse(const point2f& pos, bool down)
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

  auto image = state_.hot != nullptr ? hover_image : cursor_image;

  if(image)
  {
    sp->DrawSprite(
        *image,
        Rectf::FromPositionAnchorWidthAndHeight(
            state_.mouse, scale2f{0, 1}, image->GetWidth(), image->GetHeight()));
  }
}
