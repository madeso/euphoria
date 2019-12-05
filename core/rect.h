#ifndef CORE_RECT_H
#define CORE_RECT_H

#include "core/assert.h"
#include "core/vec2.h"
#include "core/size.h"
#include "core/range.h"
#include "core/random.h"

// Bottom, Left of screen is (0,0)
// X-axis is positive right, Y-axis is positive up

// todo: look into better names/easier discoverability for functions

namespace euphoria::core
{
    template <typename T>
    struct Rect
    {
        T left;
        T right;
        T top;
        T bottom;
        // typedef vec2<T>  vec;

        Rect() : left(0), right(0), top(0), bottom(0) {}

    private:
        Rect(T aleft, T aright, T atop, T abottom)
            : left(aleft), right(aright), top(atop), bottom(abottom)
        {}

    public:
        [[nodiscard]] static Rect
        FromLeftRightBottomTop(T aleft, T aright, T abottom, T atop)
        {
            ASSERTX(aleft <= aright, aleft, aright);
            ASSERTX(atop >= abottom, atop, abottom);
            return Rect(aleft, aright, atop, abottom);
        }

        [[nodiscard]] static Rect
        FromLeftRightTopBottom(T aleft, T aright, T atop, T abottom)
        {
            ASSERTX(aleft <= aright, aleft, aright);
            ASSERTX(atop >= abottom, atop, abottom);
            return Rect(aleft, aright, atop, abottom);
        }

        [[nodiscard]] static Rect
        FromPositionAnchorWidthAndHeight(
                const vec2<T>&   pos,
                const scale2<T>& anchor,
                float            width,
                float            height)
        {
            // todo: change anchor type to some anchor type instead
            const T left   = pos.x - width * anchor.x;
            const T bottom = pos.y - height * anchor.y;
            return FromLeftRightBottomTop(
                    left, left + width, bottom, bottom + height);
        }

        [[nodiscard]] static Rect
        FromTopLeftWidthHeight(T atop, T aleft, T width, T height)
        {
            ASSERT(width >= 0);
            ASSERT(height >= 0);
            return FromLeftRightTopBottom(
                    aleft, aleft + width, atop, atop - height);
        }

        [[nodiscard]] static Rect
        FromWidthHeight(T width, T height)
        {
            ASSERTX(width >= 0, width);
            ASSERTX(height >= 0, height);
            return FromLeftRightBottomTop(0, width, 0, height);
        }

        [[nodiscard]] static Rect
        FromWidthHeight(const Size<T>& s)
        {
            return FromWidthHeight(s.width, s.height);
        }

        [[nodiscard]] static Rect
        FromPoint(const vec2<T>& point)
        {
            return FromTopLeftWidthHeight(point.y, point.x, 0, 0);
        }

        vec2<T>
        GetBottomLeft() const
        {
            return vec2<T>(left, bottom);
        }

        vec2<T>
        GetBottomLeftOffset() const
        {
            return vec2<T>(left, bottom);
        }

        // centers this rectangle inside the other rectangle and returns it without
        // modifying this
        Rect
        CenterInsideOther(const Rect& other) const
        {
            const auto lower_left = other.GetAbsoluteCenterPos()
                                    - GetRelativeCenterPosFromBottomLeft();
            return Rect::FromTopLeftWidthHeight(
                    lower_left.y + GetHeight(),
                    lower_left.x,
                    GetWidth(),
                    GetHeight());
        }

        vec2<T>
        GetPositionFromBottomLeft(const vec2<T> v) const
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

        vec2<T>
        GetRelativeCenterPosFromBottomLeft() const
        {
            return vec2<T>(
                    GetRelativeCenterXFromBottomLeft(),
                    GetRelativeCenterYFromBottomLeft());
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

        vec2<T>
        GetAbsoluteCenterPos() const
        {
            return vec2<T>(GetAbsoluteCenterX(), GetAbsoluteCenterY());
        }

        // does this contains the argument?
        bool
        ContainsExclusive(const Rect<T>& r) const
        {
            ASSERT(IsValid());
            ASSERT(r.IsValid());

            return left < r.left && right > r.right && top > r.top
                   && bottom < r.bottom;
        }

        // on the border is NOT considered included
        bool
        ContainsExclusive(const vec2<T>& p) const
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
        ContainsInclusive(const vec2<T>& p) const
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
                    (s.width - ns.width) / 2, (s.height - ns.height) / 2);
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
        InsetCopy(T l, T r, T t, T b) const
        {
            Rect<T> ret = *this;
            ret.Inset(l, r, t, b);
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
        OffsetCopy(const vec2<T>& d) const
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
            return FromTopLeftWidthHeight(
                    newTop, newLeft, GetWidth(), GetHeight());
        }

        Rect<T>
        SetTopLeftToCopy(const vec2<T>& v) const
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
        SetBottomLeftToCopy(const vec2<T>& v) const
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

        const vec2<T>
        TopLeft() const
        {
            return vec2<T>(left, top);
        }

        const vec2<T>
        TopRight() const
        {
            return vec2<T>(right, top);
        }

        const vec2<T>
        BottomLeft() const
        {
            return vec2<T>(left, bottom);
        }

        const vec2<T>
        BottomRight() const
        {
            return vec2<T>(right, bottom);
        }

        vec2<T>
        RandomPoint(Random* random) const
        {
            const T x = random->NextRange(GetWidth());
            const T y = random->NextRange(GetHeight());
            return GetPositionFromBottomLeft(vec2<T> {x, y});
        }
    };

    template <typename T, typename R>
    const vec2<R>
    To01(const Rect<T>& r, const vec2<R>& from)
    {
        const auto x = To01(MakeRange(r.left, r.right), from.x);
        const auto y = To01(MakeRange(r.bottom, r.top), from.y);
        return vec2<R> {x, y};
    }

    template <typename T, typename R>
    const vec2<R>
    From01(const Rect<T>& r, const vec2<R>& from)
    {
        const auto x = From01(MakeRange(r.left, r.right), from.x);
        const auto y = From01(MakeRange(r.bottom, r.top), from.y);
        return vec2<R> {x, y};
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

}  // namespace euphoria::core

#endif  // CORE_RECT_H
