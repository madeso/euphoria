#include "core/interpolate.h"

float FloatTransform::Transform(float from, float v, float to) {
  return from + (to-from) * v;
}
