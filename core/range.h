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

template <typename T>
struct TRange
{
  TRange(T min, T max)
      : lower_bound(min)
      , upper_bound(max)
  {
  }

  bool
  IsWithin(T value) const
  {
    return value >= lower_bound && value <= upper_bound;
  }


  T lower_bound;
  T upper_bound;
};


#endif  // EUPHORIA_RANGE_H
