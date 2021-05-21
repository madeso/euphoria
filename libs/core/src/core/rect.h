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
    struct rect
    {
        T left;
        T right;
        T top;
        T bottom;
        // typedef vec2<T>  vec;

        rect() : left(0), right(0), top(0), bottom(0) {}

    private:
        rect(T aleft, T aright, T atop, T abottom)
            : left(aleft), right(aright), top(atop), bottom(abottom)
        {}

    public:
        template<typename Y>
        rect<Y>
        StaticCast() const
        {
            return
            rect<Y>::from_left_right_top_bottom(
                static_cast<Y>(left),
                static_cast<Y>(right),
                static_cast<Y>(top),
                static_cast<Y>(bottom)
            );
        }

        [[nodiscard]] static rect
        from_left_right_bottom_top(T aleft, T aright, T abottom, T atop)
        {
            ASSERTX(aleft <= aright, aleft, aright);
            ASSERTX(atop >= abottom, atop, abottom);
            return rect(aleft, aright, atop, abottom);
        }

        [[nodiscard]] static rect
        from_left_right_top_bottom(T aleft, T aright, T atop, T abottom)
        {
            ASSERTX(aleft <= aright, aleft, aright);
            ASSERTX(atop >= abottom, atop, abottom);
            return rect(aleft, aright, atop, abottom);
        }

        [[nodiscard]] static rect
        from_position_anchor_width_and_height(
                const vec2<T>&   pos,
                const scale2<T>& anchor,
                float            width,
                float            height)
        {
            // todo(Gustav): change anchor type to some anchor type instead
            const T left   = pos.x - width * anchor.x;
            const T bottom = pos.y - height * anchor.y;
            return from_left_right_bottom_top(
                    left, left + width, bottom, bottom + height);
        }

        [[nodiscard]]
        static
        rect
        from_bottom_left_width_height(const vec2<T>& bl, T width, T height)
        {
            ASSERT(width >= 0);
            ASSERT(height >= 0);
            return from_left_right_top_bottom
            (
                bl.x, bl.x + width, bl.y + height, bl.y
            );
        }

        [[nodiscard]] static rect
        from_top_left_width_height(const vec2<T>& topleft, T width, T height)
        {
            ASSERT(width >= 0);
            ASSERT(height >= 0);
            return from_left_right_top_bottom(
                    topleft.x, topleft.x + width, topleft.y, topleft.y - height);
        }

        [[nodiscard]] static rect
        from_width_height(T width, T height)
        {
            ASSERTX(width >= 0, width);
            ASSERTX(height >= 0, height);
            return from_left_right_bottom_top(0, width, 0, height);
        }

        [[nodiscard]] static rect
        from_width_height(const size2<T>& s)
        {
            return from_width_height(s.width, s.height);
        }

        [[nodiscard]] static rect
        from_point(const vec2<T>& point)
        {
            return from_top_left_width_height(point.y, point.x, 0, 0);
        }

        vec2<T>
        get_bottom_left() const
        {
            return vec2<T>(left, bottom);
        }

        // centers this rectangle inside the other rectangle and returns it without
        // modifying this
        rect
        center_inside_other(const rect& other) const
        {
            const auto lower_left = other.get_absolute_center_pos()
                                    - get_relative_center_pos_from_bottom_left();
            return rect::from_top_left_width_height(
                    vec2<T> {lower_left.x, lower_left.y + get_height()},
                    get_width(),
                    get_height());
        }

        vec2<T>
        get_position_from_bottom_left(const vec2<T> v) const
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

        vec2<T>
        get_relative_center_pos_from_bottom_left() const
        {
            return vec2<T>(
                    get_relative_center_x_from_bottom_left(),
                    get_relative_center_y_from_bottom_left());
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

        vec2<T>
        get_absolute_center_pos() const
        {
            return vec2<T>(get_absolute_center_x(), get_absolute_center_y());
        }

        // does this contains the argument?
        bool
        contains_exclusive(const rect<T>& r) const
        {
            ASSERT(is_valid());
            ASSERT(r.is_valid());

            return left < r.left && right > r.right && top > r.top
                   && bottom < r.bottom;
        }

        // on the border is NOT considered included
        bool
        contains_exclusive(const vec2<T>& p) const
        {
            ASSERT(is_valid());
            return contains_exclusive(p.x, p.y);
        }

        bool
        contains_exclusive(T x, T y) const
        {
            ASSERT(is_valid());
            return left < x && x < right && bottom < y && y < top;
        }

        // on the border is considered included
        bool
        contains_inclusive(const vec2<T>& p) const
        {
            return contains_inclusive(p.x, p.y);
        }

        bool
        contains_inclusive(T x, T y) const
        {
            ASSERT(is_valid());
            return left <= x && x <= right && bottom <= y && y <= top;
        }

        rect
        get_scaled_around_center_copy(T scale) const
        {
            const auto s  = get_size();
            const auto ns = s * scale;
            return inset_copy(
                    (s.width - ns.width) / 2, (s.height - ns.height) / 2);
        }

        void
        scale(T dx, T dy)
        {
            left *= dx;
            right *= dx;
            top *= dy;
            bottom *= dy;
        }

        rect
        scale_copy(T dx, T dy) const
        {
            rect r = *this;
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

        rect<T>
        inset_copy(T dx, T dy) const
        {
            rect<T> ret = *this;
            ret.inset(dx, dy);
            return ret;
        }

        rect<T>
        inset_copy(T l, T r, T t, T b) const
        {
            rect<T> ret = *this;
            ret.inset(l, r, t, b);
            return ret;
        }

        void
        extend(T dx, T dy)
        {
            inset(-dx, -dy);
        }

        rect<T>
        extend_copy(T dx, T dy) const
        {
            rect<T> ret = *this;
            ret.extend(dx, dy);
            return ret;
        }

        rect<T>
        extend_copy(T d) const
        {
            return extend_copy(d, d);
        }

        void
        include(const rect& o)
        {
            left   = min(left, o.left);
            right  = max(right, o.right);
            top    = max(top, o.top);
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

        rect<T>
        expand_copy(T expand)
        {
            rect<T> r = *this;
            r.expand(expand);
            return r;
        }

        rect<T>
        offset_copy(T dx, T dy) const
        {
            rect<T> ret = *this;
            ret.offset(dx, dy);
            return ret;
        }

        rect<T>
        offset_copy(const vec2<T>& d) const
        {
            return offset_copy(d.x, d.y);
        }

        void
        offset_to(T newLeft, T newTop)
        {
            *this = set_top_left_to_copy(newTop, newLeft);
        }

        rect<T>
        set_top_left_to_copy(T newLeft, T newTop) const
        {
            return from_top_left_width_height(
                    vec2<T>{newLeft, newTop}, get_width(), get_height());
        }

        rect<T>
        set_top_left_to_copy(const vec2<T>& v) const;

        rect<T>
        set_bottom_left_to_copy(T newLeft, T newBottom) const
        {
            return from_top_left_width_height(
                    vec2<T>{newLeft, newBottom + get_height()}, get_width(), get_height());
        }

        rect<T>
        set_bottom_left_to_copy(const vec2<T>& v) const
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

        // todo(Gustav): provide a KeepWithin, WrapWithin functions
        // like is_within below

        range<T>
        get_range_y() const
        {
            return range<T>{bottom, top};
        }

        range<T>
        get_range_x() const
        {
            return range<T>{left, right};
        }

        size2<T>
        get_size() const
        {
            return size2<T>::create_from_width_height(get_width(), get_height());
        }

        [[nodiscard]] vec2<T>
        get_top_left() const
        {
            return vec2<T>(left, top);
        }

        [[nodiscard]] vec2<T>
        get_top_right() const
        {
            return vec2<T>(right, top);
        }

        [[nodiscard]] vec2<T>
        get_bottom_right() const
        {
            return vec2<T>(right, bottom);
        }

        [[nodiscard]] vec2<T>
        get_random_point(random* random) const
        {
            const T x = get_random_in_range(random, get_width());
            const T y = get_random_in_range(random, get_height());
            return get_position_from_bottom_left(vec2<T> {x, y});
        }
    };

    template <typename T>
    rect<T> rect<T>::set_top_left_to_copy(const vec2<T>& v) const
    {
        return set_top_left_to_copy(v.x, v.y);
    }

    template <typename T, typename R>
    [[nodiscard]] vec2<R>
    to01(const rect<T>& r, const vec2<R>& from)
    {
        const auto x = to01(make_range(r.left, r.right), from.x);
        const auto y = to01(make_range(r.bottom, r.top), from.y);
        return vec2<R> {x, y};
    }

    template <typename T, typename R>
    [[nodiscard]] vec2<R>
    from01(const rect<T>& r, const vec2<R>& from)
    {
        const auto x = from01(make_range(r.left, r.right), from.x);
        const auto y = from01(make_range(r.bottom, r.top), from.y);
        return vec2<R> {x, y};
    }

    template<typename T>
    bool
    is_within(const rect<T>& r, const vec2<T>& p)
    {
        return
            is_within(r.get_range_x(), p.x) &&
            is_within(r.get_range_y(), p.y) ;
    }

    template <typename T>
    bool
    operator==(const rect<T>& lhs, const rect<T>& rhs)
    {
        return
            lhs.left == rhs.left &&
            lhs.right == rhs.right &&
            lhs.top == rhs.top &&
            lhs.bottom == rhs.bottom ;
    }

    template <typename S, typename T>
    S&
    operator<<(S& s, const rect<T>& r)
    {
        s << "(" << r.left << ", " << r.bottom << " / " << r.get_width() << " x "
          << r.get_height() << ")";
        return s;
    }

    using recti = rect<int>;
    using rectf = rect<float>;

}
