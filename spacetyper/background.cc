#include "spacetyper/background.h"

#include <random>

#include "spacetyper/spriterender.h"
#include "spacetyper/texture.h"

namespace {
std::mt19937& Generator() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return gen;
}
}

std::uniform_real_distribution<float> GetDistribution(float window,
                                                      float texture) {
  const float d = texture / 2.0f;
  const float minimum = 10;
  std::uniform_real_distribution<float> rwidth(-d + minimum,
                                               window + d - minimum);
  return rwidth;
}

Background::Background(int count, int width, int height, Texture2d* texture,
                       float speed, Layer* layer)
    : width_(width), height_(height) , speed_(speed) {
  auto rwidth = GetDistribution(width, texture->width());
  auto rheight = GetDistribution(height, texture->height());

  positions_.reserve(count);
  for (int i = 0; i < count; ++i) {
    vec2f p(rwidth(Generator()), rheight(Generator()));
    Sprite sp(texture, p);
    positions_.push_back(sp);
    layer->Add(&*positions_.rbegin());
  }
}

void Background::Update(float delta) {
  for (Sprite& sp : positions_) {
    vec2f p = sp.GetPosition();
    p.y += delta * speed_;

    if (p.y > height_ + sp.GetHeight() / 2) {
      auto rwidth = GetDistribution(width_, sp.GetWidth());
      p.y = -sp.GetHeight() / 2;
      p.x = rwidth(Generator());
    }

    sp.SetPosition(p);
  }
}
