#ifndef CORE_AXISANGLE_H
#define CORE_AXISANGLE_H

#include <iostream>

#include "core/vec3.h"
#include "core/angle.h"

class AxisAngle
{
 public:
  static const AxisAngle
  RightHandAround(const unit3f& axis, const Angle& angle);

  /** a unit-vector.
  */
  unit3f axis;

  /** rotation according to right-hand rule.
  */
  Angle angle;

 private:
  AxisAngle(const unit3f& ax, const Angle& ang);
};

std::ostream&
operator<<(std::ostream& stream, const AxisAngle& aa);

#endif  // CORE_AXISANGLE_H
