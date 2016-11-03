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
                       float speed)
    : width_(width), height_(height), speed_(speed), texture_(texture) {
  positions_.reserve(count);

  auto rwidth = GetDistribution(width, texture->width());
  auto rheight = GetDistribution(height, texture->height());

  for (int i = 0; i < count; ++i) {
    glm::vec2 p(rwidth(Generator()), rheight(Generator()));
    positions_.push_back(p);
  }
}

void Background::Update(float delta) {
  auto rwidth = GetDistribution(width_, texture_->width());
  for (glm::vec2& p : positions_) {
    p.y += delta * speed_;

    if (p.y > height_ + texture_->height() / 2) {
      p.y = -texture_->height() / 2;
      p.x = rwidth(Generator());
    }
  }
}

void Background::Render(SpriteRenderer* renderer) const {
  for (const glm::vec2& p : positions_) {
    renderer->DrawSprite(*texture_, p);
  }
}
