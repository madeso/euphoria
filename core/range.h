#ifndef EUPHORIA_RANGE_H
#define EUPHORIA_RANGE_H

class Range
{
 public:
  Range(float min, float max);
  explicit Range(float max);

  float lower_bound;
  float upper_bound;
};


float
From01(const Range& range, float value);

float
To01(const Range& range, float value);

float
RemapTo(const Range& from, const Range& to, float value);

float
Get360Angular(const Range& range, float value);

bool
IsWithin(const Range& range, float value);

float
KeepWithin(const Range& range, float value);

float
Wrap(const Range& range, float value);


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

  T
  KeepWithin(T value) const
  {
    if(value <= lower_bound ) return lower_bound;
    if(value >= upper_bound ) return upper_bound;
    return value;
  }


  T lower_bound;
  T upper_bound;
};


#endif  // EUPHORIA_RANGE_H
