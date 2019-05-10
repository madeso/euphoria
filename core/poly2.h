
#ifndef CORE_POLY2_H
#define CORE_POLY2_H

#include <vector>
#include <cassert>

#include "core/vec2.h"

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2  on the line
//            <0 for P2  right of the line
//    See: Algorithm 1 "Area of Triangles and Polygons"
// todo: move to the line class

template <typename T>
int
isLeft(const vec2<T>& P0, const vec2<T>& P1, const vec2<T>& P2)
{
  return (P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y);
}

// todo: a poly is a closed path, so rename to path
// todo: add more features
// sebastian lague: bezier support https://github.com/SebLague/Path-Creator
// paper.js, smooth and simpify(path fitter) path functions: https://github.com/paperjs/paper.js/blob/develop/src/path/PathFitter.js https://github.com/paperjs/paper.js/blob/develop/src/path/PathItem.js#L466
template <typename T>
class Poly2
{
 public:
  typedef vec2<T> vec;

  Poly2()
  {
  }
  Poly2(const std::vector<vec>& ps)
      : points(ps)
  {
  }

  size_t
  Size() const
  {
    return points.size();
  }

  const vec& operator[](int i) const
  {
    assert(i >= 0);
    const int ii = i % Size();
    return points[ii];
  }

  void
  Add(const vec& p)
  {
    points.push_back(p);
  }

  bool
  IsInside(const vec2<T>& p) const
  {
    int wn = GetWindingNumberForPointInPoly(p);
    return wn != 0;
  }

  vec
  GetCenterPosition() const
  {
    vec r{0, 0};
    int count = 0;
    for(const vec& p : points)
    {
      r += p;
      count += 1;
    }
    if(count == 0)
    {
      return r;
    }
    else
    {
      return r / static_cast<T>(count);
    }
  }

  void
  Scale(T scale, const vec& center)
  {
    for(vec& p : points)
    {
      p = (p - center) * scale + center;
    }
  }

  void
  Expand(T scale, const vec& center)
  {
    for(vec& p : points)
    {
      p += vec::FromTo(center, p).GetNormalized() * scale;
    }
  }

  // wn_PnPoly(): winding number test for a point in a polygon
  //      Input:   P = a point
  //      Return:  wn = the winding number (=0 only when P is outside)
  int
  GetWindingNumberForPointInPoly(const vec2<T>& P) const
  {
    int wn = 0;  // the  winding number counter

    const Poly2<T>& V = *this;
    const int       n = V.Size();

    // loop through all edges of the polygon
    for(int i = 0; i < n; i++)
    {  // edge from V[i] to  V[i+1]
      if(V[i].y <= P.y)
      {                                      // start y <= P.y
        if(V[i + 1].y > P.y)                 // an upward crossing
          if(isLeft(V[i], V[i + 1], P) > 0)  // P left of  edge
            ++wn;                            // have  a valid up intersect
      }
      else
      {                                      // start y > P.y (no test needed)
        if(V[i + 1].y <= P.y)                // a downward crossing
          if(isLeft(V[i], V[i + 1], P) < 0)  // P right of  edge
            --wn;                            // have  a valid down intersect
      }
    }
    return wn;
  }

 private:
  std::vector<vec> points;
};

typedef Poly2<float> Poly2f;

#endif  // CORE_POLY_H
