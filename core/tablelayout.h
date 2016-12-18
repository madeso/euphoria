#ifndef CORE_TABLELAYOUT_H
#define CORE_TABLELAYOUT_H

#include "core/numeric.h"

#include <cassert>
#include <vector>

// positive = absolute pixel size
// zero = invalid
// negative = scaling size
// if available size < min_size scaling areas will have "zero" and absolute areas will be resized
template <typename T>
class PerformTableLayout{
 public:
  const std::vector<T> pieces;
  const T zero;
  T min_size;
  T total_percentage;

  PerformTableLayout(const std::vector<T>& p, T z = 0) : pieces(p), zero(z), min_size(0), total_percentage(0) {
    assert(zero <= 0);

    for(T f: pieces) {
      if( f > 0 ) min_size += f;
      else total_percentage += -f;
    }
  }

  std::vector<T> GetLayoutTableLayout(T size) {
    assert(size >= 0);

    const T size_left = size - min_size;
    const T fixed_scale = min_size < size ? 1 : size/min_size;

    std::vector<T> ret;
    ret.reserve(pieces.size());
    for(T f: pieces) {
      if( f > 0 ) {
        ret.push_back(f * fixed_scale);
      }
      else {
        if( size_left <= 0 ) {
          // no more room, add 0
          ret.push_back(zero);
        }
        else {
          const T p = (-f / total_percentage);
          const T size = p * size_left;
          ret.push_back(size);
        }
      }
    }
    return ret;
  }
};


#endif  // CORE_TABLELAYOUT_H