#include "eu/base/rect.h"

namespace eu
{

    Rect::Rect()
        : left(0)
        , right(0)
        , top(0)
        , bottom(0)
    {
    }

    [[nodiscard]] Rect
    Rect::from_left_right_top_bottom(float left_side, float right_side, float top_side, float bottom_side)
    {
        ASSERTX(left_side <= right_side, left_side, right_side);
        ASSERTX(top_side >= bottom_side, top_side, bottom_side);
        return {left_side, right_side, top_side, bottom_side};
    }

    [[nodiscard]] Rect
    Rect::from_bottom_left_size(const v2& bl, const v2& size)
    {
        ASSERT(size.x >= 0);
        ASSERT(size.y >= 0);
        return from_left_right_top_bottom
        (
            bl.x, bl.x + size.x,
            bl.y + size.y, bl.y
        );
    }

    [[nodiscard]] Rect
    Rect::from_top_left_size(const v2& tl, const v2& size)
    {
        ASSERT(size.x >= 0);
        ASSERT(size.y >= 0);
        return from_left_right_top_bottom
        (
            tl.x, tl.x + size.x,
            tl.y, tl.y - size.y
        );
    }

    [[nodiscard]] Rect
    Rect::from_size(const v2& size)
    {
        ASSERTX(size.x >= 0, size.x);
        ASSERTX(size.y >= 0, size.y);
        return from_left_right_bottom_top(0, size.x, 0, size.y);
    }

    [[nodiscard]] Rect
    Rect::from_point(const v2& point)
    {
        return from_top_left_size(point, {0.0f, 0.0f});
    }

    v2
    Rect::get_bottom_left() const
    {
        return {left, bottom};
    }

    Rect
    Rect::from_center_inside_other(const Rect& self, const Rect& other)
    {
        const auto size = self.get_size();
        const auto lower_left = other.get_center_pos() - self.get_relative_center_pos();
        return Rect::from_top_left_size
        (
            {lower_left.x, lower_left.y + size.y}, size
        );
    }

    Rect
    Rect::from_scaled(const Rect& self, float scale)
    {
        const auto s = self.get_size();
        const auto ns = s * scale;
        return self.with_inset
        ({
            (s.x - ns.x) / 2,
            (s.y - ns.y) / 2
        });
    }

    v2
    Rect::get_relative_center_pos() const
    {
        const auto size = get_size();
        const auto cx = size.x / 2;
        const auto cy = size.y / 2;
        return {cx, cy};
    }

    v2
    Rect::get_center_pos() const
    {
        const auto rc = get_relative_center_pos();
        const auto cx = left + rc.x;
        const auto cy = bottom + rc.y;
        return {cx, cy};
    }

    bool
    Rect::contains_exclusive(const Rect& r) const
    {
        ASSERT(is_valid());
        ASSERT(r.is_valid());

        return left < r.left
            && right > r.right
            && top > r.top
            && bottom < r.bottom
            ;
    }

    void
    Rect::inset(const Lrtb& lrtb)
    {
        left += lrtb.left;
        right -= lrtb.right;
        top -= lrtb.top;
        bottom += lrtb.bottom;
    }

    Rect
    Rect::with_inset(const Lrtb& lrtb) const
    {
        Rect ret = *this;
        ret.inset(lrtb);
        return ret;
    }

    void
    Rect::extend(const Rect& o)
    {
        left = min(left, o.left);
        right = max(right, o.right);
        top = max(top, o.top);
        bottom = min(bottom, o.bottom);
    }

    
    [[nodiscard]] bool
    Rect::is_empty() const
    {
        return left >= right || top <= bottom;
    }

    
    [[nodiscard]] bool
    Rect::is_valid() const
    {
        const auto size = get_size();
        return size.x >= 0 && size.y >= 0;
    }

    
    void
    Rect::translate(const v2& d)
    {
        left += d.x;
        right += d.x;
        top += d.y;
        bottom += d.y;
    }

    Rect
    Rect::with_translate(const v2& d) const
    {
        Rect ret = *this;
        ret.translate(d);
        return ret;
    }


    Rect
    Rect::with_top_left_at(const v2& p) const
    {
        return from_top_left_size
        (
            p,
            get_size()
        );
    }

    Rect
    Rect::with_bottom_left_at(const v2& p) const
    {
        const auto size = get_size();
        return from_top_left_size
        (
            v2{p.x, p.y + size.y},
            size
        );
    }

    [[nodiscard]] Rect
    Rect::with_offset(const v2& p) const
    {
        return from_top_left_size(get_top_left() + p, get_size());
    }


    [[nodiscard]] Rect
    Rect::with_scale(float hor, float vert) const
    {
        return from_left_right_top_bottom(left * hor, right * hor, top * vert, bottom * vert);
    }

    void
    Rect::set_empty()
    {
        left = right = top = bottom = 0;
    }


    v2
    Rect::get_size() const
    {
        const auto height = top - bottom;
        const auto width = right - left;
        return { width, height };
    }

    
    R<float>
    Rect::get_range_y() const
    {
        return {bottom, top};
    }

    R<float>
    Rect::get_range_x() const
    {
        return {left, right};
    }

    [[nodiscard]] v2
    Rect::get_top_left() const
    {
        return {left, top};
    }

    [[nodiscard]] v2
    Rect::get_top_right() const
    {
        return {right, top};
    }

    [[nodiscard]] v2
    Rect::get_bottom_right() const
    {
        return {right, bottom};
    }

    Rect Rect::cut(RectCut side, float amount, float spacing)
    {
        switch (side)
        {
        case RectCut::left: return cut_left(amount, spacing);
        case RectCut::right: return cut_right(amount, spacing);
        case RectCut::top: return cut_top(amount, spacing);
        case RectCut::bottom: return cut_bottom(amount, spacing);
        default:
            DIE("invalid rectcut");
            return cut_left(amount);
        }
    }

    Rect lbrt(float l, float b, float r, float t)
    {
        return Rect::from_left_right_top_bottom(l, r, t, b);
    }

    Rect Rect::cut_top(float amount, float v_spacing)
    {
        float old_max_y = top;
        top = std::max(left, top - amount);
        Rect area = lbrt(left, top, right, old_max_y);
        top -= v_spacing;
        return area;
    }

    Rect Rect::cut_right(float amount, float h_spacing)
    {
        float old_max_x = right;
        right = std::max(left, right - amount);
        Rect area = lbrt(right, bottom, old_max_x, top);
        right -= h_spacing;
        return area;
    }

    Rect Rect::cut_bottom(float amount, float v_spacing)
    {
        float old_min_y = bottom;
        bottom = std::min(top, bottom + amount);
        Rect area = lbrt(left, old_min_y, right, bottom);
        bottom += v_spacing;
        return area;
    }

    Rect Rect::cut_left(float amount, float h_spacing)
    {
        float old_min_x = left;
        left = std::min(right, left + amount);
        Rect area = lbrt(old_min_x, bottom, left, top);
        left += h_spacing;
        return area;
    }

    Rect Rect::get_top(float amount) const
    {
        float new_min_y = std::max(bottom, top - amount);
        return lbrt(left, new_min_y, right, top);
    }

    Rect Rect::get_right(float amount) const
    {
        float new_min_x = std::max(left, right - amount);
        return lbrt(new_min_x, bottom, right, top);
    }

    Rect Rect::get_bottom(float amount) const
    {
        float new_max_y = std::min(top, bottom + amount);
        return lbrt(left, bottom, right, new_max_y);
    }

    Rect Rect::get_left(float amount) const
    {
        float new_max_x = std::min(right, left + amount);
        return lbrt(left, bottom, new_max_x, top);
    }

    Rect Rect::add_top(float amount) const
    {
        return lbrt(left, top, right, top + amount);
    }

    Rect Rect::add_right(float amount) const
    {
        return lbrt(right, bottom, right + amount, top);
    }

    Rect Rect::add_bottom(float amount) const
    {
        return lbrt(left, bottom - amount, right, bottom);
    }

    Rect Rect::add_left(float amount) const
    {
        return lbrt(left - amount, bottom, left, top);
    }


    ///////////////////////////////////////////////////////////////////////////


    [[nodiscard]] v2
    to_01(const Rect& r, const v2& from)
    {
        const auto x = to01(make_range(r.left, r.right), from.x);
        const auto y = to01(make_range(r.bottom, r.top), from.y);
        return {x, y};
    }

    [[nodiscard]] v2
    from_01(const Rect& r, const v2& from)
    {
        const auto x = from_01(make_range(r.left, r.right), from.x);
        const auto y = from_01(make_range(r.bottom, r.top), from.y);
        return {x, y};
    }

    bool
    is_within(const v2& p, const Rect& r)
    {
        ASSERT(r.is_valid());
        return r.left <= p.x && p.x <= r.right
            && r.bottom <= p.y && p.y <= r.top
            ;
    }

    std::string to_string(const Rect& r)
    {
        const auto size = r.get_size();
        return fmt::format("({}, {} / {} x {})", r.left, r.bottom, size.x, size.y);
    }
}
