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
  static Rect FromLeftTopWidthHeight(T aleft, T atop, T width, T height) { return Rect(aleft, aleft + width, atop + height); }

  T GetCenterX() const { return left + GetWidth() / 2;}
  T GetCenterY() const { return top + GetHeight() / 2; }

  bool Contains(T left, T right, T top, T bottom) const { return Contains(FromLeftRightTopBottom(left, right, top, bottom)); }

  bool 	Contains(const Rect<T>& r) const {
    return left < r.left && right > r.right
        && top < r.top && bottom > r.bottom;
  }

  bool Contains(T x, T y) const {
    return left < x && right > x
           && top < y && bottom > y;
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

  void OffsetTo(T newLeft, T newTop) {
    *this = FromLeftTopWidthHeight(newLeft, newTop, GetWidth(), GetHeight());
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