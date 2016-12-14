#ifndef SPACETYPER_ANGLE_H
#define SPACETYPER_ANGLE_H

class Angle
{
 public:
  const float inDegrees() const;
  const float inRadians() const;

  static const Angle FromDegrees(float deg);
  static const Angle FromRadians(float rad);

  static const Angle FromPercentOf360(const float percent);
  static const Angle Zero();

  const Angle GetWrapped() const;
  void Wrap();

  void operator+=(const Angle& rhs);

  void operator-=(const Angle& rhs);

  void operator*=(const float r);
 private:
  explicit Angle(float rad);
  float mRad;
};


const float Sin(const Angle& ang);
const float Cos(const Angle& ang);
const float Tan(const Angle& ang);
const Angle Asin(const float v);
const Angle Acos(const float v);
const Angle Atan(const float v);

const Angle operator+(const Angle& lhs, const Angle& rhs);
const Angle operator-(const Angle& lhs, const Angle& rhs);
const Angle operator*(const Angle& lhs, const float rhs);
const Angle operator*(const float rhs, const Angle& lhs);

#endif  // SPACETYPER_ANGLE_H