#ifndef CORE_RECT_H
#define CORE_RECT_H

#include "core/assert.h"
#include "core/vec2.h"
#include "core/size.h"
#include "core/line2.h"

// Bottom, Left of screen is (0,0)
// X-axis is positive right, Y-axis is positive up

// todo: look into better names/easier discoverability for functions

template <typename T>
class Rect
{
 public:
  T left;
  T right;
  T top;
  T bottom;

  typedef line2<T> Line;
  typedef vec2<T, StoreValue<T>> vec;

  Rect()
      : left(0)
      , right(0)
      , top(0)
      , bottom(0)
  {
  }

 private:
  Rect(T aleft, T aright, T atop, T abottom)
      : left(aleft)
      , right(aright)
      , top(atop)
      , bottom(abottom)
  {
  }

 public:
  static Rect
  FromLeftRightBottomTop(T aleft, T aright, T abottom, T atop)
  {
    ASSERTX(aleft <= aright, aleft, aright);
    ASSERTX(atop >= abottom, atop, abottom);
    return Rect(aleft, aright, atop, abottom);
  }
  static Rect
  FromLeftRightTopBottom(T aleft, T aright, T atop, T abottom)
  {
    ASSERTX(aleft <= aright, aleft, aright);
    ASSERTX(atop >= abottom, atop, abottom);
    return Rect(aleft, aright, atop, abottom);
  }
  static Rect
  FromPositionAnchorWidthAndHeight(
      const vec2<T, StoreValue<T>>& pos,
      const vec2<T, StoreValue<T>>& anchor,
      float width,
      float height)
  {
    const T left   = pos.x - width * anchor.x;
    const T bottom = pos.y - height * anchor.y;
    return FromLeftRightBottomTop(left, left + width, bottom, bottom + height);
  }
  static Rect
  FromTopLeftWidthHeight(T atop, T aleft, T width, T height)
  {
    ASSERT(width >= 0);
    ASSERT(height >= 0);
    return FromLeftRightTopBottom(aleft, aleft + width, atop, atop - height);
  }
  static Rect
  FromWidthHeight(T width, T height)
  {
    ASSERTX(width >= 0, width);
    ASSERTX(height >= 0, height);
    return FromLeftRightBottomTop(0, width, 0, height);
  }
  static Rect
  FromWidthHeight(const Size<T>& s)
  {
    return FromWidthHeight(s.GetWidth(), s.GetHeight());
  }
  static Rect
  FromPoint(const vec& point)
  {
    return FromTopLeftWidthHeight(point.y, point.x, 0, 0);
  }

  vec2<T, StoreValue<T>>
  GetBottomLeft() const
  {
    return vec2<T, StoreValue<T>>(left, bottom);
  }

  // centers this rectangle inside the other rectangle and returns it without
  // modifying this
  Rect
  CenterInsideOther(const Rect& other) const
  {
    const auto lower_left =
        other.GetAbsoluteCenterPos() - GetRelativeCenterPosFromBottomLeft();
    return Rect::FromTopLeftWidthHeight(
        lower_left.y + GetHeight(), lower_left.x, GetWidth(), GetHeight());
  }

  vec2<T, StoreValue<T>>
  GetPositionFromBottomLeft(const vec2<T, StoreValue<T>> v) const
  {
    return GetBottomLeft() + v;
  }

  T
  GetRelativeCenterXFromBottomLeft() const
  {
    return GetWidth() / 2;
  }

  T
  GetRelativeCenterYFromBottomLeft() const
  {
    return GetHeight() / 2;
  }

  vec2<T, StoreValue<T>>
  GetRelativeCenterPosFromBottomLeft() const
  {
    return vec2<T, StoreValue<T>>(
        GetRelativeCenterXFromBottomLeft(), GetRelativeCenterYFromBottomLeft());
  }

  T
  GetAbsoluteCenterX() const
  {
    return left + GetRelativeCenterXFromBottomLeft();
  }

  T
  GetAbsoluteCenterY() const
  {
    return bottom + GetRelativeCenterYFromBottomLeft();
  }

  vec2<T, StoreValue<T>>
  GetAbsoluteCenterPos() const
  {
    return vec2<T, StoreValue<T>>(GetAbsoluteCenterX(), GetAbsoluteCenterY());
  }

  // does this contains the argument?
  bool
  ContainsExclusive(const Rect<T>& r) const
  {
    ASSERT(IsValid());
    ASSERT(r.IsValid());

    return left < r.left && right > r.right && top > r.top && bottom < r.bottom;
  }

  // on the border is NOT considered included
  template <typename TStorage>
  bool
  ContainsExclusive(const vec2<T, TStorage>& p) const
  {
    ASSERT(IsValid());
    return ContainsExclusive(p.x, p.y);
  }

  bool
  ContainsExclusive(T x, T y) const
  {
    ASSERT(IsValid());
    return left < x && x < right && bottom < y && y < top;
  }

  // on the border is considered included
  template <typename TStorage>
  bool
  ContainsInclusive(const vec2<T, TStorage>& p) const
  {
    return ContainsInclusive(p.x, p.y);
  }

  bool
  ContainsInclusive(T x, T y) const
  {
    ASSERT(IsValid());
    return left <= x && x <= right && bottom <= y && y <= top;
  }

  Rect
  GetScaledAroundCenterCopy(T scale) const
  {
    const auto s  = GetSize();
    const auto ns = s * scale;
    return InsetCopy(
        (s.GetWidth() - ns.GetWidth()) / 2,
        (s.GetHeight() - ns.GetHeight()) / 2);
  }

  void
  Scale(T dx, T dy)
  {
    left *= dx;
    right *= dx;
    top *= dy;
    bottom *= dy;
  }

  Rect
  ScaleCopy(T dx, T dy) const
  {
    Rect r = *this;
    r.Scale(dx, dy);
    return r;
  }

  void
  Inset(T dx, T dy)
  {
    left += dx;
    right -= dx;
    top -= dy;
    bottom += dy;
  }

  void
  Inset(T l, T r, T t, T b)
  {
    left += l;
    right -= r;
    top -= t;
    bottom += b;
  }

  Rect<T>
  InsetCopy(T dx, T dy) const
  {
    Rect<T> ret = *this;
    ret.Inset(dx, dy);
    return ret;
  }

  Rect<T>
  InsetCopy(T left, T right, T top, T bottom) const
  {
    Rect<T> ret = *this;
    ret.Inset(left, right, top, bottom);
    return ret;
  }

  void
  Extend(T dx, T dy)
  {
    Inset(-dx, -dy);
  }

  Rect<T>
  ExtendCopy(T dx, T dy) const
  {
    Rect<T> ret = *this;
    ret.Extend(dx, dy);
    return ret;
  }

  Rect<T>
  ExtendCopy(T d) const
  {
    return ExtendCopy(d, d);
  }

  void
  Include(const Rect& o)
  {
    left   = Min(left, o.left);
    right  = Max(right, o.right);
    top    = Max(top, o.top);
    bottom = Min(bottom, o.bottom);
  }

  // Returns true if the rectangle is empty (left >= right or top <= bottom)
  bool
  IsEmpty() const
  {
    return left >= right || top <= bottom;
  }

  // doe this represent a rectangle? A 0 width/height is also considered valid
  bool
  IsValid() const
  {
    return GetWidth() >= 0 && GetHeight() >= 0;
  }

  // Translate
  void
  Offset(T dx, T dy)
  {
    left += dx;
    right += dx;
    top += dy;
    bottom += dy;
  }

  void
  Expand(T expand)
  {
    left -= expand;
    right += expand;
    top -= expand;
    bottom += expand;
  }

  Rect<T>
  ExpandCopy(T expand)
  {
    Rect<T> r = *this;
    r.Expand(expand);
    return r;
  }

  Rect<T>
  OffsetCopy(T dx, T dy) const
  {
    Rect<T> ret = *this;
    ret.Offset(dx, dy);
    return ret;
  }

  template <typename TStorage>
  Rect<T>
  OffsetCopy(const vec2<T, TStorage>& d) const
  {
    return OffsetCopy(d.x, d.y);
  }

  void
  OffsetTo(T newLeft, T newTop)
  {
    *this = SetTopLeftToCopy(newTop, newLeft);
  }

  Rect<T>
  SetTopLeftToCopy(T newLeft, T newTop) const
  {
    return FromTopLeftWidthHeight(newTop, newLeft, GetWidth(), GetHeight());
  }

  Rect<T>
  SetTopLeftToCopy(const vec& v) const
  {
    return SetTopLeftToCopy(v.x, v.y);
  }

  Rect<T>
  SetBottomLeftToCopy(T newLeft, T newBottom) const
  {
    return FromTopLeftWidthHeight(
        newBottom + GetHeight(), newLeft, GetWidth(), GetHeight());
  }

  Rect<T>
  SetBottomLeftToCopy(const vec& v) const
  {
    return SetBottomLeftToCopy(v.x, v.y);
  }

  void
  SetEmpty()
  {
    left = right = top = bottom = 0;
  }

  // todo: add union and intersection functions

  T
  GetHeight() const
  {
    return top - bottom;
  }

  T
  GetWidth() const
  {
    return right - left;
  }

  Size<T>
  GetSize() const
  {
    return Size<T>::FromWidthHeight(GetWidth(), GetHeight());
  }

  const vec
  TopLeft() const
  {
    return vec(left, top);
  }

  const vec
  TopRight() const
  {
    return vec(right, top);
  }

  const vec
  BottomLeft() const
  {
    return vec(left, bottom);
  }

  const vec
  BottomRight() const
  {
    return vec(right, bottom);
  }

  const Line
  LeftEdge() const
  {
    return Line::FromTo(TopLeft(), BottomLeft());
  }

  const Line
  RightEdge() const
  {
    return Line::FromTo(TopRight(), BottomRight());
  }

  const Line
  TopEdge() const
  {
    return Line::FromTo(TopLeft(), TopRight());
  }

  const Line
  BottomEdge() const
  {
    return Line::FromTo(BottomLeft(), BottomRight());
  }

  const typename Line::Collision
  GetPointOnEdge(const Line& line) const
  {
    return Line::Collision::GetClosestCollision(
        Line::Collision::GetClosestCollision(
            line.GetIntersection(TopEdge()),
            line.GetIntersection(BottomEdge())),
        Line::Collision::GetClosestCollision(
            line.GetIntersection(LeftEdge()),
            line.GetIntersection(RightEdge())));
  }
};


template <typename T, typename R, typename TStorage>
const vec2<R, StoreValue<T>>
To01(const Rect<T>& rect, const vec2<R, TStorage>& from)
{
  const auto x = To01(rect.left, from.x, rect.right);
  const auto y = To01(rect.bottom, from.y, rect.top);
  return vec2<R, StoreValue<T>>{x, y};
}

template <typename T, typename R, typename TStorage>
const vec2<R, StoreValue<T>>
From01(const Rect<T>& rect, const vec2<R, TStorage>& from)
{
  const auto x = From01(rect.left, from.x, rect.right);
  const auto y = From01(rect.bottom, from.y, rect.top);
  return vec2<R, StoreValue<T>>{x, y};
}

template <typename S, typename T>
S&
operator<<(S& s, const Rect<T>& r)
{
  s << "(" << r.left << ", " << r.bottom << " / " << r.GetWidth() << " x "
    << r.GetHeight() << ")";
  return s;
}

typedef Rect<int>   Recti;
typedef Rect<float> Rectf;

#endif  // CORE_RECT_H
