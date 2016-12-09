#ifndef SPACETYPER_RECT_H
#define SPACETYPER_RECT_H

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
  static Rect FromLeftTopWidthHeight(T aleft, T atop, T width, T height) { return Rect(aleft, aleft + width, atop, atop + height); }
  static Rect FromWidthHeight(T width, T height) { return Rect(0, width, 0, height); }

  T GetRelativeCenterX() const { return GetWidth() / 2;}
  T GetRelativeCenterY() const { return GetHeight() / 2; }

  T GetAbsoluteCenterX() const { return left + GetRelativeCenterX();}
  T GetAbsoluteCenterY() const { return top + GetRelativeCenterY(); }

  bool ContainsExclusive(const Rect<T>& r) const {
    return left < r.left && right > r.right
        && top < r.top && bottom > r.bottom;
  }

  bool ContainsInclusive(const Rect<T>& r) const {
    return left <= r.left && right >= r.right
           && top <= r.top && bottom >= r.bottom;
  }

  // on the border is NOT considered included
  bool ContainsExclusive(T x, T y) const {
    return left < x && right > x
           && top < y && bottom > y;
  }

  // on the border is considered included
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

  Rect<T> Inseted(T dx, T dy) const {
    Rect<T> ret = *this;
    ret.Inset(dx, dy);
    return ret;
  }

  // Returns true if the rectangle is empty (left >= right or top >= bottom)
  bool IsEmpty() const {
    return left >= right || top >= bottom;
  }

  void Offset(T dx, T dy) {
    left += dx;
    right += dx;
    top += dy;
    bottom += dy;
  }

  Rect<T> Offseted(T dx, T dy) const {
    Rect<T> ret = *this;
    ret.Offset(dx, dy);
    return ret;
  }

  void OffsetTo(T newLeft, T newTop) {
    *this = FromLeftTopWidthHeight(newLeft, newTop, GetWidth(), GetHeight());
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

#endif  // SPACETYPER_RECT_H