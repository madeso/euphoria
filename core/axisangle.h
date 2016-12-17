#ifndef CORE_AXISANGLE_H
#define CORE_AXISANGLE_H

#include "core/vec3.h"
#include "core/angle.h"

class AxisAngle
{
 public:
  static const AxisAngle RightHandAround(const vec3f::Unit& axis, const Angle& angle);

  /** a unit-vector.
  */
  vec3f axis;

  /** rotation according to right-hand rule.
  */
  Angle angle;

 private:
  AxisAngle(const vec3f::Unit& axis, const Angle& angle);
};

#endif  // CORE_AXISANGLE_H
