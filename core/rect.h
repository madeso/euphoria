#ifndef CORE_RECT_H
#define CORE_RECT_H

#include "core/vec2.h"
#include "core/size.h"

template <typename T>
class Rect {
 public:
  T left;
  T right;
  T bottom;
  T top;

  Rect() : left(0), right(0), bottom(0), top(0) {}

 private:
  Rect(T aleft, T aright, T atop, T abottom) : left(aleft), right(aright), bottom(abottom), top(atop) { }
 public:
  static Rect FromLeftRightTopBottom(T aleft, T aright, T atop, T abottom) { return Rect(aleft, aright, atop, abottom); }
  static Rect FromTopLeftWidthHeight(T atop, T aleft, T width, T height) { return FromLeftRightTopBottom(aleft, aleft + width, atop, atop + height); }
  static Rect FromWidthHeight(T width, T height) { return FromLeftRightTopBottom(0, width, 0, height); }
  static Rect FromWidthHeight(const Size<T>& s) { return FromWidthHeight(s.GetWidth(), s.GetHeight()); }

  T GetX() const {
    return left;
  }

  T GetY() const {
    return top;
  }

  vec2<T> GetPosition() const {
    return vec2<T>(GetX(), GetY());
  }

  T GetRelativeCenterX() const { return GetWidth() / 2;}
  T GetRelativeCenterY() const { return GetHeight() / 2; }
  vec2<T> GetRelativeCenterPos() const { return vec2<T>(GetRelativeCenterX(), GetRelativeCenterY()); }

  T GetAbsoluteCenterX() const { return left + GetRelativeCenterX();}
  T GetAbsoluteCenterY() const { return top + GetRelativeCenterY(); }
  vec2<T> GetAbsoluteCenterPos() const { return vec2<T>(GetAbsoluteCenterX(), GetAbsoluteCenterY()); }

  bool ContainsExclusive(const Rect<T>& r) const {
    return left < r.left && right > r.right
        && top < r.top && bottom > r.bottom;
  }

  // on the border is NOT considered included
  bool ContainsExclusive(const vec2<T>& p) const {
    return ContainsExclusive(p.x, p.y);
  }

  bool ContainsExclusive(T x, T y) const {
    return left < x && right > x
           && top < y && bottom > y;
  }

  // on the border is considered included
  bool ContainsInclusive(const vec2<T>& p) const {
    return ContainsInclusive(p.x, p.y);
  }

  bool ContainsInclusive(T x, T y) const {
    return left <= x && right >= x
           && top <= y && bottom >= y;
  }

  void Inset(T dx, T dy) {
    left += dx;
    right -= dx;
    top += dy;
    bottom -= dy;
  }

  Rect<T> InsetCopy(T dx, T dy) const {
    Rect<T> ret = *this;
    ret.Inset(dx, dy);
    return ret;
  }

  void Extend(T dx, T dy) {
    Inset(-dx, -dy);
  }

  Rect<T> ExtendCopy(T dx, T dy) const {
    Rect<T> ret = *this;
    ret.Extend(dx, dy);
    return ret;
  }

  Rect<T> ExtendCopy(T d) const {
    return ExtendCopy(d, d);
  }

  void Include(const Rect& o) {
    left = Min(left, o.left);
    right = Max(right, o.right);
    top = Min(top, o.top);
    bottom = Max(bottom, o.bottom);
  }

  // Returns true if the rectangle is empty (left >= right or top >= bottom)
  bool IsEmpty() const {
    return left >= right || top >= bottom;
  }

  // Translate
  void Offset(T dx, T dy) {
    left += dx;
    right += dx;
    top += dy;
    bottom += dy;
  }

  Rect<T> OffsetCopy(T dx, T dy) const {
    Rect<T> ret = *this;
    ret.Offset(dx, dy);
    return ret;
  }
  Rect<T> OffsetCopy(const vec2<T>& d) const {
    return OffsetCopy(d.x, d.y);
  }

  void OffsetTo(T newLeft, T newTop) {
    *this = FromTopLeftWidthHeight(newTop, newLeft, GetWidth(), GetHeight());
  }

  Rect<T> OffsetToCopy(T newLeft, T newTop) const {
    Rect<T> ret = *this;
    ret.OffsetTo(newLeft, newTop);
    return ret;
  }

  void SetEmpty() {
    left = right = top = bottom = 0;
  }

  // todo: add union and intersection functions

  T GetHeight() const {
    return bottom - top;
  }

  T GetWidth() const {
    return right - left;
  }
};

typedef Rect<int> Recti;
typedef Rect<float> Rectf;

#endif  // CORE_RECT_H