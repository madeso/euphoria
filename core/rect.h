#ifndef CORE_RECT_H
#define CORE_RECT_H

#include "core/assert.h"
#include "core/vec2.h"
#include "core/size.h"
#include "core/line2.h"
#include "core/range.h"

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
  // typedef vec2<T>  vec;

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
      const point2<T>& pos, const scale2<T>& anchor, float width, float height)
  {
    // todo: change anchor type to some anchor type instead
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
  FromPoint(const point2<T>& point)
  {
    return FromTopLeftWidthHeight(point.y, point.x, 0, 0);
  }

  point2<T>
  GetBottomLeft() const
  {
    return point2<T>(left, bottom);
  }

  Vec2<T>
  GetBottomLeftOffset() const
  {
    return Vec2<T>(left, bottom);
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

  point2<T>
  GetPositionFromBottomLeft(const point2<T> v) const
  {
    return GetBottomLeftOffset() + v;
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

  point2<T>
  GetRelativeCenterPosFromBottomLeft() const
  {
    return point2<T>(
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

  point2<T>
  GetAbsoluteCenterPos() const
  {
    return point2<T>(GetAbsoluteCenterX(), GetAbsoluteCenterY());
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
  bool
  ContainsExclusive(const point2<T>& p) const
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
  bool
  ContainsInclusive(const point2<T>& p) const
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

  Rect<T>
  OffsetCopy(const Vec2<T>& d) const
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
  SetTopLeftToCopy(const point2<T>& v) const
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
  SetBottomLeftToCopy(const point2<T>& v) const
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

  const point2<T>
  TopLeft() const
  {
    return point2<T>(left, top);
  }

  const point2<T>
  TopRight() const
  {
    return point2<T>(right, top);
  }

  const point2<T>
  BottomLeft() const
  {
    return point2<T>(left, bottom);
  }

  const point2<T>
  BottomRight() const
  {
    return point2<T>(right, bottom);
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


template <typename T, typename R>
const point2<R>
To01(const Rect<T>& rect, const point2<R>& from)
{
  const auto x = Range(rect.left, rect.right).To01(from.x);
  const auto y = Range(rect.bottom, rect.top).To01(from.y);
  return point2<R>{x, y};
}

template <typename T, typename R>
const point2<R>
From01(const Rect<T>& rect, const point2<R>& from)
{
  const auto x = Range(rect.left, rect.right).From01(from.x);
  const auto y = Range(rect.bottom, rect.top).From01(from.y);
  return point2<R>{x, y};
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
