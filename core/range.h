#ifndef EUPHORIA_RANGE_H
#define EUPHORIA_RANGE_H


class Range
{
 public:
  Range(float min, float max);

  explicit Range(float max);

  float
  From01(float value) const;

  float
  To01(float value) const;

  float
  RemapTo(const Range& other, float value) const;

  float
  Get360Angular(float value) const;

  bool
  IsWithin(float value) const;

  float
  KeepWithin(float value) const;

  float
  Wrap(float value) const;

  float lower_bound;
  float upper_bound;
};


#endif  // EUPHORIA_RANGE_H
