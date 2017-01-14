#include "gui/root.h"
#include "gui/load.h"

Root::Root(const Sizef& size) : size_(size) {

}

bool Root::Load(Font* font, const std::string& path, TextureCache* cache) {
  const bool result = ::Load(&state_, font, &container_, path, cache);
  if( result ) {
    container_.DoLayout(Rectf::FromWidthHeight(size_));
  }
  return result;
}

void Root::Step() {
  container_.Step();
}

void Root::Render(SpriteRenderer* sp) const {
  container_.Render(sp);
}
