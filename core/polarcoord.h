#ifndef EUPHORIA_POLARCOORD_H
#define EUPHORIA_POLARCOORD_H

#include "core/angle.h"
#include "core/vec3.h"

// source: http://mathworld.wolfram.com/SphericalCoordinates.html

class PolarCoord {
 public:
  PolarCoord(Angle azimuthal, Angle polar);

  vec3f::Unit ToCartesian() const;
 private:
  Angle azimuthal_; // longitude, 0-2pi
  Angle polar_; // zenith, colatitude, 0-pi
};


#endif //EUPHORIA_POLARCOORD_H
