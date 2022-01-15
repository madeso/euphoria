#pragma once

#include "core/assert.h"
#include "core/vec2.h"
#include "core/size2.h"
#include "core/range.h"
#include "core/random.h"

// Bottom, Left of screen is (0,0)
// X-axis is positive right, Y-axis is positive up

// todo(Gustav): look into better names/easier discoverability for functions

namespace euphoria::core
{
    template <typename T>
    struct Rect
    {
        T left;
        T right;
        T top;
        T bottom;

        Rect()
            : left(0)
            , right(0)
            , top(0)
            , bottom(0)
        {
        }

    private:
        Rect(T left_side, T right_side, T top_side, T bottom_side)
            : left(left_side)
            , right(right_side)
            , top(top_side)
            , bottom(bottom_side)
        {
        }

    public:
        template<typename Y>
        explicit operator Rect<Y>() const
        {
            return Rect<Y>::from_left_right_top_bottom
            (
                static_cast<Y>(left),
                static_cast<Y>(right),
                static_cast<Y>(top),
                static_cast<Y>(bottom)
            );
        }

        [[nodiscard]] static Rect
        from_left_right_bottom_top(T left_side, T right_side, T bottom_side, T top_side)
        {
            ASSERTX(left_side <= right_side, left_side, right_side);
            ASSERTX(top_side >= bottom_side, top_side, bottom_side);
            return Rect(left_side, right_side, top_side, bottom_side);
        }

        [[nodiscard]] static Rect
        from_left_right_top_bottom(T left_side, T right_side, T top_side, T bottom_side)
        {
            ASSERTX(left_side <= right_side, left_side, right_side);
            ASSERTX(top_side >= bottom_side, top_side, bottom_side);
            return Rect(left_side, right_side, top_side, bottom_side);
        }

        [[nodiscard]] static Rect
        from_position_anchor_width_and_height
        (
            const Vec2<T>& pos,
            const Scale2<T>& anchor,
            T width,
            T height
        )
        {
            // todo(Gustav): change anchor type to some anchor type instead
            const T left = pos.x - width * anchor.x;
            const T bottom = pos.y - height * anchor.y;
            return from_left_right_bottom_top
            (
                left, left + width,
                bottom, bottom + height
            );
        }

        [[nodiscard]]
        static
        Rect
        from_bottom_left_width_height(const Vec2<T>& bl, T width, T height)
        {
            ASSERT(width >= 0);
            ASSERT(height >= 0);
            return from_left_right_top_bottom
            (
                bl.x, bl.x + width,
                bl.y + height, bl.y
            );
        }

        [[nodiscard]] static Rect
        from_top_left_width_height(const Vec2<T>& topleft, T width, T height)
        {
            ASSERT(width >= 0);
            ASSERT(height >= 0);
            return from_left_right_top_bottom
            (
                topleft.x, topleft.x + width,
                topleft.y, topleft.y - height
            );
        }

        [[nodiscard]] static Rect
        from_width_height(T width, T height)
        {
            ASSERTX(width >= 0, width);
            ASSERTX(height >= 0, height);
            return from_left_right_bottom_top(0, width, 0, height);
        }

        [[nodiscard]] static Rect
        from_width_height(const Size2<T>& s)
        {
            return from_width_height(s.width, s.height);
        }

        [[nodiscard]] static Rect
        from_point(const Vec2<T>& point)
        {
            return from_top_left_width_height(point.y, point.x, 0, 0);
        }

        Vec2<T>
        get_bottom_left() const
        {
            return Vec2<T>(left, bottom);
        }

        // centers this rectangle inside the other rectangle and returns it without
        // modifying this
        Rect
        center_inside_other(const Rect& other) const
        {
            const auto lower_left = other.get_absolute_center_pos()
                                    - get_relative_center_pos_from_bottom_left();
            return Rect::from_top_left_width_height
            (
                Vec2<T> {lower_left.x, lower_left.y + get_height()},
                get_width(),
                get_height()
            );
        }

        Vec2<T>
        get_position_from_bottom_left(const Vec2<T> v) const
        {
            return get_bottom_left() + v;
        }

        T
        get_relative_center_x_from_bottom_left() const
        {
            return get_width() / 2;
        }

        T
        get_relative_center_y_from_bottom_left() const
        {
            return get_height() / 2;
        }

        Vec2<T>
        get_relative_center_pos_from_bottom_left() const
        {
            return Vec2<T>
            (
                get_relative_center_x_from_bottom_left(),
                get_relative_center_y_from_bottom_left()
            );
        }

        T
        get_absolute_center_x() const
        {
            return left + get_relative_center_x_from_bottom_left();
        }

        T
        get_absolute_center_y() const
        {
            return bottom + get_relative_center_y_from_bottom_left();
        }

        Vec2<T>
        get_absolute_center_pos() const
        {
            return Vec2<T>(get_absolute_center_x(), get_absolute_center_y());
        }

        // does this contains the argument?
        bool
        contains_exclusive(const Rect<T>& r) const
        {
            ASSERT(is_valid());
            ASSERT(r.is_valid());

            return left < r.left
                && right > r.right
                && top > r.top
                && bottom < r.bottom
                ;
        }

        // on the border is NOT considered included
        bool
        contains_exclusive(const Vec2<T>& p) const
        {
            ASSERT(is_valid());
            return contains_exclusive(p.x, p.y);
        }

        bool
        contains_exclusive(T x, T y) const
        {
            ASSERT(is_valid());
            return left < x
                && x < right
                && bottom < y
                && y < top
                ;
        }

        // on the border is considered included
        bool
        contains_inclusive(const Vec2<T>& p) const
        {
            return contains_inclusive(p.x, p.y);
        }

        bool
        contains_inclusive(T x, T y) const
        {
            ASSERT(is_valid());
            return left <= x
                && x <= right
                && bottom <= y
                && y <= top
                ;
        }

        Rect
        get_scaled_around_center_copy(T scale) const
        {
            const auto s = get_size();
            const auto ns = s * scale;
            return inset_copy
            (
                (s.width - ns.width) / 2,
                (s.height - ns.height) / 2
            );
        }

        void
        scale(T dx, T dy)
        {
            left *= dx;
            right *= dx;
            top *= dy;
            bottom *= dy;
        }

        Rect
        scale_copy(T dx, T dy) const
        {
            Rect r = *this;
            r.scale(dx, dy);
            return r;
        }

        void
        inset(T dx, T dy)
        {
            left += dx;
            right -= dx;
            top -= dy;
            bottom += dy;
        }

        void
        inset(T l, T r, T t, T b)
        {
            left += l;
            right -= r;
            top -= t;
            bottom += b;
        }

        Rect<T>
        inset_copy(T dx, T dy) const
        {
            Rect<T> ret = *this;
            ret.inset(dx, dy);
            return ret;
        }

        Rect<T>
        inset_copy(T l, T r, T t, T b) const
        {
            Rect<T> ret = *this;
            ret.inset(l, r, t, b);
            return ret;
        }

        void
        extend(T dx, T dy)
        {
            inset(-dx, -dy);
        }

        Rect<T>
        extend_copy(T dx, T dy) const
        {
            Rect<T> ret = *this;
            ret.extend(dx, dy);
            return ret;
        }

        Rect<T>
        extend_copy(T d) const
        {
            return extend_copy(d, d);
        }

        void
        include(const Rect& o)
        {
            left = min(left, o.left);
            right = max(right, o.right);
            top = max(top, o.top);
            bottom = min(bottom, o.bottom);
        }

        // Returns true if the rectangle is empty (left >= right or top <= bottom)
        [[nodiscard]] bool
        is_empty() const
        {
            return left >= right || top <= bottom;
        }

        // does this represent a rectangle? A 0 width/height is also considered valid
        [[nodiscard]] bool
        is_valid() const
        {
            return get_width() >= 0 && get_height() >= 0;
        }

        // Translate
        void
        offset(T dx, T dy)
        {
            left += dx;
            right += dx;
            top += dy;
            bottom += dy;
        }

        void
        expand(T expand)
        {
            left -= expand;
            right += expand;
            top -= expand;
            bottom += expand;
        }

        Rect<T>
        expand_copy(T expand)
        {
            Rect<T> r = *this;
            r.expand(expand);
            return r;
        }

        Rect<T>
        offset_copy(T dx, T dy) const
        {
            Rect<T> ret = *this;
            ret.offset(dx, dy);
            return ret;
        }

        Rect<T>
        offset_copy(const Vec2<T>& d) const
        {
            return offset_copy(d.x, d.y);
        }

        void
        offset_to(T new_left, T new_top)
        {
            *this = set_top_left_to_copy(new_top, new_left);
        }

        Rect<T>
        set_top_left_to_copy(T new_left, T new_top) const
        {
            return from_top_left_width_height
            (
                Vec2<T>{new_left, new_top},
                get_width(),
                get_height()
            );
        }

        Rect<T>
        set_top_left_to_copy(const Vec2<T>& v) const;

        Rect<T>
        set_bottom_left_to_copy(T new_left, T new_bottom) const
        {
            return from_top_left_width_height
            (
                Vec2<T>{new_left, new_bottom + get_height()},
                get_width(),
                get_height()
            );
        }

        Rect<T>
        set_bottom_left_to_copy(const Vec2<T>& v) const
        {
            return set_bottom_left_to_copy(v.x, v.y);
        }

        void
        set_empty()
        {
            left = right = top = bottom = 0;
        }

        // todo(Gustav): add union and intersection functions

        T
        get_height() const
        {
            return top - bottom;
        }

        T
        get_width() const
        {
            return right - left;
        }

        // todo(Gustav): provide a keep_within, WrapWithin functions
        // like is_within below

        Range<T>
        get_range_y() const
        {
            return Range<T>{bottom, top};
        }

        Range<T>
        get_range_x() const
        {
            return Range<T>{left, right};
        }

        Size2<T>
        get_size() const
        {
            return Size2<T>::create_from_width_height(get_width(), get_height());
        }

        [[nodiscard]] Vec2<T>
        get_top_left() const
        {
            return Vec2<T>(left, top);
        }

        [[nodiscard]] Vec2<T>
        get_top_right() const
        {
            return Vec2<T>(right, top);
        }

        [[nodiscard]] Vec2<T>
        get_bottom_right() const
        {
            return Vec2<T>(right, bottom);
        }

        [[nodiscard]] Vec2<T>
        get_random_point(Random* random) const
        {
            const T x = get_random_in_range(random, get_width());
            const T y = get_random_in_range(random, get_height());
            return get_position_from_bottom_left(Vec2<T> {x, y});
        }
    };

    template <typename T>
    Rect<T> Rect<T>::set_top_left_to_copy(const Vec2<T>& v) const
    {
        return set_top_left_to_copy(v.x, v.y);
    }

    template <typename T, typename R>
    [[nodiscard]] Vec2<R>
    to01(const Rect<T>& r, const Vec2<R>& from)
    {
        const auto x = to01(make_range(r.left, r.right), from.x);
        const auto y = to01(make_range(r.bottom, r.top), from.y);
        return Vec2<R> {x, y};
    }

    template <typename T, typename R>
    [[nodiscard]] Vec2<R>
    from01(const Rect<T>& r, const Vec2<R>& from)
    {
        const auto x = from01(make_range(r.left, r.right), from.x);
        const auto y = from01(make_range(r.bottom, r.top), from.y);
        return Vec2<R> {x, y};
    }

    template<typename T>
    bool
    is_within(const Rect<T>& r, const Vec2<T>& p)
    {
        return
            is_within(r.get_range_x(), p.x) &&
            is_within(r.get_range_y(), p.y) ;
    }

    template <typename T>
    bool
    operator==(const Rect<T>& lhs, const Rect<T>& rhs)
    {
        return lhs.left == rhs.left
            && lhs.right == rhs.right
            && lhs.top == rhs.top
            && lhs.bottom == rhs.bottom
            ;
    }

    template <typename S, typename T>
    S&
    operator<<(S& s, const Rect<T>& r)
    {
        s
            << "("
            << r.left << ", " << r.bottom << " / "
            << r.get_width() << " x " << r.get_height()
            << ")"
            ;
        return s;
    }

    using Recti = Rect<int>;
    using Rectf = Rect<float>;

}
