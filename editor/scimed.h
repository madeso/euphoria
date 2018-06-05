#ifndef EUPHORIA_SCIMED_H
#define EUPHORIA_SCIMED_H

#include <memory>

#include "gaf_scalingsprite.h"

class Texture2d;

struct Scimed
{
  scalingsprite::ScalingSprite scaling;

  std::shared_ptr<Texture2d> texture;

  bool
  Run();
};


#endif  // EUPHORIA_SCIMED_H
