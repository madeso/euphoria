#include "core/rect.h"

#include "core/random.h"


namespace euphoria::core
{

    Rectf::Rectf()
        : left(0)
        , right(0)
        , top(0)
        , bottom(0)
    {
    }

    
    Rectf::Rectf(float left_side, float right_side, float top_side, float bottom_side)
        : left(left_side)
        , right(right_side)
        , top(top_side)
        , bottom(bottom_side)
    {
    }

    Recti
    Rectf::to_i() const
    {
        return Recti::from_left_right_top_bottom
        (
            static_cast<int>(left),
            static_cast<int>(right),
            static_cast<int>(top),
            static_cast<int>(bottom)
        );
    }

    [[nodiscard]] Rectf
    Rectf::from_left_right_bottom_top(float left_side, float right_side, float bottom_side, float top_side)
    {
        ASSERTX(left_side <= right_side, left_side, right_side);
        ASSERTX(top_side >= bottom_side, top_side, bottom_side);
        return Rectf(left_side, right_side, top_side, bottom_side);
    }

    [[nodiscard]] Rectf
    Rectf::from_left_right_top_bottom(float left_side, float right_side, float top_side, float bottom_side)
    {
        ASSERTX(left_side <= right_side, left_side, right_side);
        ASSERTX(top_side >= bottom_side, top_side, bottom_side);
        return Rectf(left_side, right_side, top_side, bottom_side);
    }

    [[nodiscard]] Rectf
    Rectf::from_position_anchor_width_and_height
    (
        const vec2f& pos,
        const Scale2f& anchor,
        float width,
        float height
    )
    {
        // todo(Gustav): change anchor type to some anchor type instead
        const float left = pos.x - width * anchor.x;
        const float bottom = pos.y - height * anchor.y;
        return from_left_right_bottom_top
        (
            left, left + width,
            bottom, bottom + height
        );
    }

    [[nodiscard]] Rectf
    Rectf::from_bottom_left_width_height(const vec2f& bl, float width, float height)
    {
        ASSERT(width >= 0);
        ASSERT(height >= 0);
        return from_left_right_top_bottom
        (
            bl.x, bl.x + width,
            bl.y + height, bl.y
        );
    }

    [[nodiscard]] Rectf
    Rectf::from_top_left_width_height(const vec2f& topleft, float width, float height)
    {
        ASSERT(width >= 0);
        ASSERT(height >= 0);
        return from_left_right_top_bottom
        (
            topleft.x, topleft.x + width,
            topleft.y, topleft.y - height
        );
    }

    [[nodiscard]] Rectf
    Rectf::from_width_height(float width, float height)
    {
        ASSERTX(width >= 0, width);
        ASSERTX(height >= 0, height);
        return from_left_right_bottom_top(0, width, 0, height);
    }

    [[nodiscard]] Rectf
    Rectf::from_width_height(const size2f& s)
    {
        return from_width_height(s.width, s.height);
    }

    [[nodiscard]] Rectf
    Rectf::from_point(const vec2f& point)
    {
        return from_top_left_width_height(point, 0.0f, 0.0f);
    }

    vec2f
    Rectf::get_bottom_left() const
    {
        return vec2f(left, bottom);
    }

    Rectf
    Rectf::center_inside_other(const Rectf& other) const
    {
        const auto lower_left = other.get_absolute_center_pos() - get_relative_center_pos_from_bottom_left();
        return Rectf::from_top_left_width_height
        (
            vec2f {lower_left.x, lower_left.y + get_height()},
            get_width(),
            get_height()
        );
    }

    vec2f
    Rectf::get_position_from_bottom_left(const vec2f& v) const
    {
        return get_bottom_left() + v;
    }

    float
    Rectf::get_relative_center_x_from_bottom_left() const
    {
        return get_width() / 2;
    }

    float
    Rectf::get_relative_center_y_from_bottom_left() const
    {
        return get_height() / 2;
    }

    vec2f
    Rectf::get_relative_center_pos_from_bottom_left() const
    {
        return vec2f
        (
            get_relative_center_x_from_bottom_left(),
            get_relative_center_y_from_bottom_left()
        );
    }

    float
    Rectf::get_absolute_center_x() const
    {
        return left + get_relative_center_x_from_bottom_left();
    }

    float
    Rectf::get_absolute_center_y() const
    {
        return bottom + get_relative_center_y_from_bottom_left();
    }

    vec2f
    Rectf::get_absolute_center_pos() const
    {
        return vec2f(get_absolute_center_x(), get_absolute_center_y());
    }

    bool
    Rectf::contains_exclusive(const Rectf& r) const
    {
        ASSERT(is_valid());
        ASSERT(r.is_valid());

        return left < r.left
            && right > r.right
            && top > r.top
            && bottom < r.bottom
            ;
    }

    bool
    Rectf::contains_exclusive(const vec2f& p) const
    {
        ASSERT(is_valid());
        return contains_exclusive(p.x, p.y);
    }

    bool
    Rectf::contains_exclusive(float x, float y) const
    {
        ASSERT(is_valid());
        return left < x
            && x < right
            && bottom < y
            && y < top
            ;
    }

    
    bool
    Rectf::contains_inclusive(const vec2f& p) const
    {
        return contains_inclusive(p.x, p.y);
    }

    
    bool
    Rectf::contains_inclusive(float x, float y) const
    {
        ASSERT(is_valid());
        return left <= x
            && x <= right
            && bottom <= y
            && y <= top
            ;
    }

    Rectf
    Rectf::get_scaled_around_center_copy(float scale) const
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
    Rectf::scale(float dx, float dy)
    {
        left *= dx;
        right *= dx;
        top *= dy;
        bottom *= dy;
    }


    Rectf
    Rectf::scale_copy(float dx, float dy) const
    {
        Rectf r = *this;
        r.scale(dx, dy);
        return r;
    }


    void
    Rectf::inset(float dx, float dy)
    {
        left += dx;
        right -= dx;
        top -= dy;
        bottom += dy;
    }


    void
    Rectf::inset(float l, float r, float t, float b)
    {
        left += l;
        right -= r;
        top -= t;
        bottom += b;
    }

    Rectf
    Rectf::inset_copy(float dx, float dy) const
    {
        Rectf ret = *this;
        ret.inset(dx, dy);
        return ret;
    }

    Rectf
    Rectf::inset_copy(float l, float r, float t, float b) const
    {
        Rectf ret = *this;
        ret.inset(l, r, t, b);
        return ret;
    }

    void
    Rectf::extend(float dx, float dy)
    {
        inset(-dx, -dy);
    }


    Rectf
    Rectf::extend_copy(float dx, float dy) const
    {
        Rectf ret = *this;
        ret.extend(dx, dy);
        return ret;
    }


    Rectf
    Rectf::extend_copy(float d) const
    {
        return extend_copy(d, d);
    }

    void
    Rectf::include(const Rectf& o)
    {
        left = min(left, o.left);
        right = max(right, o.right);
        top = max(top, o.top);
        bottom = min(bottom, o.bottom);
    }

    
    [[nodiscard]] bool
    Rectf::is_empty() const
    {
        return left >= right || top <= bottom;
    }

    
    [[nodiscard]] bool
    Rectf::is_valid() const
    {
        return get_width() >= 0 && get_height() >= 0;
    }

    
    void
    Rectf::offset(float dx, float dy)
    {
        left += dx;
        right += dx;
        top += dy;
        bottom += dy;
    }


    void
    Rectf::expand(float expand)
    {
        left -= expand;
        right += expand;
        top -= expand;
        bottom += expand;
    }


    Rectf
    Rectf::expand_copy(float expand) const
    {
        Rectf r = *this;
        r.expand(expand);
        return r;
    }

    Rectf
    Rectf::offset_copy(float dx, float dy) const
    {
        Rectf ret = *this;
        ret.offset(dx, dy);
        return ret;
    }

    Rectf
    Rectf::offset_copy(const vec2f& d) const
    {
        return offset_copy(d.x, d.y);
    }

    void
    Rectf::offset_to(float new_left, float new_top)
    {
        *this = set_top_left_to_copy(new_top, new_left);
    }


    Rectf
    Rectf::set_top_left_to_copy(float new_left, float new_top) const
    {
        return from_top_left_width_height
        (
            vec2f{new_left, new_top},
            get_width(),
            get_height()
        );
    }

    Rectf
    Rectf::set_top_left_to_copy(const vec2f& v) const
    {
        return set_top_left_to_copy(v.x, v.y);
    }

    Rectf
    Rectf::set_bottom_left_to_copy(float new_left, float new_bottom) const
    {
        return from_top_left_width_height
        (
            vec2f{new_left, new_bottom + get_height()},
            get_width(),
            get_height()
        );
    }


    Rectf
    Rectf::set_bottom_left_to_copy(const vec2f& v) const
    {
        return set_bottom_left_to_copy(v.x, v.y);
    }


    void
    Rectf::set_empty()
    {
        left = right = top = bottom = 0;
    }


    float
    Rectf::get_height() const
    {
        return top - bottom;
    }

    float
    Rectf::get_width() const
    {
        return right - left;
    }

    
    Range<float>
    Rectf::get_range_y() const
    {
        return Range<float>{bottom, top};
    }

    Range<float>
    Rectf::get_range_x() const
    {
        return Range<float>{left, right};
    }

    size2f
    Rectf::get_size() const
    {
        return size2f::create_from_width_height(get_width(), get_height());
    }

    [[nodiscard]] vec2f
    Rectf::get_top_left() const
    {
        return vec2f(left, top);
    }

    [[nodiscard]] vec2f
    Rectf::get_top_right() const
    {
        return vec2f(right, top);
    }

    [[nodiscard]] vec2f
    Rectf::get_bottom_right() const
    {
        return vec2f(right, bottom);
    }


    ///////////////////////////////////////////////////////////////////////////


    

    Recti::Recti()
        : left(0)
        , right(0)
        , top(0)
        , bottom(0)
    {
    }

    Recti::Recti(int left_side, int right_side, int top_side, int bottom_side)
        : left(left_side)
        , right(right_side)
        , top(top_side)
        , bottom(bottom_side)
    {
    }

    Rectf
    Recti::to_f() const
    {
        return Rectf::from_left_right_top_bottom
        (
            static_cast<float>(left),
            static_cast<float>(right),
            static_cast<float>(top),
            static_cast<float>(bottom)
        );
    }

    [[nodiscard]] Recti
    Recti::from_left_right_bottom_top(int left_side, int right_side, int bottom_side, int top_side)
    {
        ASSERTX(left_side <= right_side, left_side, right_side);
        ASSERTX(top_side >= bottom_side, top_side, bottom_side);
        return Recti(left_side, right_side, top_side, bottom_side);
    }

    [[nodiscard]] Recti
    Recti::from_left_right_top_bottom(int left_side, int right_side, int top_side, int bottom_side)
    {
        ASSERTX(left_side <= right_side, left_side, right_side);
        ASSERTX(top_side >= bottom_side, top_side, bottom_side);
        return Recti(left_side, right_side, top_side, bottom_side);
    }

    [[nodiscard]] Recti
    Recti::from_position_anchor_width_and_height
    (
        const vec2i& pos,
        const Scale2f& anchor,
        int width,
        int height
    )
    {
        // todo(Gustav): change anchor type to some anchor type instead
        const int left = pos.x - static_cast<int>(static_cast<float>(width) * anchor.x);
        const int bottom = pos.y - static_cast<int>(static_cast<float>(height) * anchor.y);
        return from_left_right_bottom_top
        (
            left, left + width,
            bottom, bottom + height
        );
    }

    [[nodiscard]] Recti
    Recti::from_bottom_left_width_height(const vec2i& bl, int width, int height)
    {
        ASSERT(width >= 0);
        ASSERT(height >= 0);
        return from_left_right_top_bottom
        (
            bl.x, bl.x + width,
            bl.y + height, bl.y
        );
    }


    [[nodiscard]] Recti
    Recti::from_top_left_width_height(const vec2i& topleft, int width, int height)
    {
        ASSERT(width >= 0);
        ASSERT(height >= 0);
        return from_left_right_top_bottom
        (
            topleft.x, topleft.x + width,
            topleft.y, topleft.y - height
        );
    }

    [[nodiscard]] Recti
    Recti::from_width_height(int width, int height)
    {
        ASSERTX(width >= 0, width);
        ASSERTX(height >= 0, height);
        return from_left_right_bottom_top(0, width, 0, height);
    }

    [[nodiscard]] Recti
    Recti::from_width_height(const size2i& s)
    {
        return from_width_height(s.width, s.height);
    }

    [[nodiscard]] Recti
    Recti::from_point(const vec2i& point)
    {
        return from_top_left_width_height(point, 0, 0);
    }

    vec2i
    Recti::get_bottom_left() const
    {
        return vec2i(left, bottom);
    }

    
    Recti
    Recti::center_inside_other(const Recti& other) const
    {
        const auto lower_left = other.get_absolute_center_pos() - get_relative_center_pos_from_bottom_left();
        return Recti::from_top_left_width_height
        (
            vec2i {lower_left.x, lower_left.y + get_height()},
            get_width(),
            get_height()
        );
    }

    vec2i
    Recti::get_position_from_bottom_left(const vec2i& v) const
    {
        return get_bottom_left() + v;
    }

    int
    Recti::get_relative_center_x_from_bottom_left() const
    {
        return get_width() / 2;
    }

    int
    Recti::get_relative_center_y_from_bottom_left() const
    {
        return get_height() / 2;
    }

    vec2i
    Recti::get_relative_center_pos_from_bottom_left() const
    {
        return vec2i
        (
            get_relative_center_x_from_bottom_left(),
            get_relative_center_y_from_bottom_left()
        );
    }

    int
    Recti::get_absolute_center_x() const
    {
        return left + get_relative_center_x_from_bottom_left();
    }

    int
    Recti::get_absolute_center_y() const
    {
        return bottom + get_relative_center_y_from_bottom_left();
    }

    vec2i
    Recti::get_absolute_center_pos() const
    {
        return vec2i(get_absolute_center_x(), get_absolute_center_y());
    }

    bool
    Recti::contains_exclusive(const Recti& r) const
    {
        ASSERT(is_valid());
        ASSERT(r.is_valid());

        return left < r.left
            && right > r.right
            && top > r.top
            && bottom < r.bottom
            ;
    }

    bool
    Recti::contains_exclusive(const vec2i& p) const
    {
        ASSERT(is_valid());
        return contains_exclusive(p.x, p.y);
    }

    bool
    Recti::contains_exclusive(int x, int y) const
    {
        ASSERT(is_valid());
        return left < x
            && x < right
            && bottom < y
            && y < top
            ;
    }

    bool
    Recti::contains_inclusive(const vec2i& p) const
    {
        return contains_inclusive(p.x, p.y);
    }

    bool
    Recti::contains_inclusive(int x, int y) const
    {
        ASSERT(is_valid());
        return left <= x
            && x <= right
            && bottom <= y
            && y <= top
            ;
    }

    Recti
    Recti::get_scaled_around_center_copy(int scale) const
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
    Recti::scale(int dx, int dy)
    {
        left *= dx;
        right *= dx;
        top *= dy;
        bottom *= dy;
    }

    Recti
    Recti::scale_copy(int dx, int dy) const
    {
        Recti r = *this;
        r.scale(dx, dy);
        return r;
    }

    void
    Recti::inset(int dx, int dy)
    {
        left += dx;
        right -= dx;
        top -= dy;
        bottom += dy;
    }

    void
    Recti::inset(int l, int r, int t, int b)
    {
        left += l;
        right -= r;
        top -= t;
        bottom += b;
    }

    Recti
    Recti::inset_copy(int dx, int dy) const
    {
        Recti ret = *this;
        ret.inset(dx, dy);
        return ret;
    }

    Recti
    Recti::inset_copy(int l, int r, int t, int b) const
    {
        Recti ret = *this;
        ret.inset(l, r, t, b);
        return ret;
    }

    void
    Recti::extend(int dx, int dy)
    {
        inset(-dx, -dy);
    }

    Recti
    Recti::extend_copy(int dx, int dy) const
    {
        Recti ret = *this;
        ret.extend(dx, dy);
        return ret;
    }

    Recti
    Recti::extend_copy(int d) const
    {
        return extend_copy(d, d);
    }

    void
    Recti::include(const Recti& o)
    {
        left = min(left, o.left);
        right = max(right, o.right);
        top = max(top, o.top);
        bottom = min(bottom, o.bottom);
    }

    [[nodiscard]] bool
    Recti::is_empty() const
    {
        return left >= right || top <= bottom;
    }

    [[nodiscard]] bool
    Recti::is_valid() const
    {
        return get_width() >= 0 && get_height() >= 0;
    }

    void
    Recti::offset(int dx, int dy)
    {
        left += dx;
        right += dx;
        top += dy;
        bottom += dy;
    }

    void
    Recti::expand(int expand)
    {
        left -= expand;
        right += expand;
        top -= expand;
        bottom += expand;
    }

    Recti
    Recti::expand_copy(int expand) const
    {
        Recti r = *this;
        r.expand(expand);
        return r;
    }

    Recti
    Recti::offset_copy(int dx, int dy) const
    {
        Recti ret = *this;
        ret.offset(dx, dy);
        return ret;
    }

    Recti
    Recti::offset_copy(const vec2i& d) const
    {
        return offset_copy(d.x, d.y);
    }

    void
    Recti::offset_to(int new_left, int new_top)
    {
        *this = set_top_left_to_copy(new_top, new_left);
    }

    Recti
    Recti::set_top_left_to_copy(int new_left, int new_top) const
    {
        return from_top_left_width_height
        (
            vec2i{new_left, new_top},
            get_width(),
            get_height()
        );
    }

    Recti
    Recti::set_top_left_to_copy(const vec2i& v) const
    {
        return set_top_left_to_copy(v.x, v.y);
    }

    Recti
    Recti::set_bottom_left_to_copy(int new_left, int new_bottom) const
    {
        return from_top_left_width_height
        (
            vec2i{new_left, new_bottom + get_height()},
            get_width(),
            get_height()
        );
    }

    Recti
    Recti::set_bottom_left_to_copy(const vec2i& v) const
    {
        return set_bottom_left_to_copy(v.x, v.y);
    }

    void
    Recti::set_empty()
    {
        left = right = top = bottom = 0;
    }


    int
    Recti::get_height() const
    {
        return top - bottom;
    }

    int
    Recti::get_width() const
    {
        return right - left;
    }


    Range<int>
    Recti::get_range_y() const
    {
        return Range<int>{bottom, top};
    }

    Range<int>
    Recti::get_range_x() const
    {
        return Range<int>{left, right};
    }

    size2i
    Recti::get_size() const
    {
        return size2i::create_from_width_height(get_width(), get_height());
    }

    [[nodiscard]] vec2i
    Recti::get_top_left() const
    {
        return vec2i(left, top);
    }

    [[nodiscard]] vec2i
    Recti::get_top_right() const
    {
        return vec2i(right, top);
    }

    [[nodiscard]] vec2i
    Recti::get_bottom_right() const
    {
        return vec2i(right, bottom);
    }
        

    ///////////////////////////////////////////////////////////////////////////


    [[nodiscard]] vec2f
    to01(const Rectf& r, const vec2f& from)
    {
        const auto x = to01(make_range(r.left, r.right), from.x);
        const auto y = to01(make_range(r.bottom, r.top), from.y);
        return vec2f {x, y};
    }

    [[nodiscard]] vec2f
    from_01(const Rectf& r, const vec2f& from)
    {
        const auto x = from_01(make_range(r.left, r.right), from.x);
        const auto y = from_01(make_range(r.bottom, r.top), from.y);
        return vec2f {x, y};
    }

    bool
    is_within(const Rectf& r, const vec2f& p)
    {
        return
            is_within(r.get_range_x(), p.x) &&
            is_within(r.get_range_y(), p.y) ;
    }

    std::ostream&
    operator<<(std::ostream& s, const Rectf& r)
    {
        s
            << "("
            << r.left << ", " << r.bottom << " / "
            << r.get_width() << " x " << r.get_height()
            << ")"
            ;
        return s;
    }

    [[nodiscard]] vec2f
    to01(const Recti& r, const vec2i& from)
    {
        const auto x = to01<float>
        (
            make_range<float>(static_cast<float>(r.left), static_cast<float>(r.right)),
            static_cast<float>(from.x)
        );
        const auto y = to01<float>
        (
            make_range<float>(static_cast<float>(r.bottom), static_cast<float>(r.top)),
            static_cast<float>(from.y)
        );
        return vec2f{x, y};
    }

    [[nodiscard]] vec2i
    from_01(const Recti& r, const vec2f& from)
    {
        const auto x = from_01(make_range(r.left, r.right), from.x);
        const auto y = from_01(make_range(r.bottom, r.top), from.y);
        return vec2i {x, y};
    }

    bool
    is_within(const Recti& r, const vec2i& p)
    {
        return
            is_within(r.get_range_x(), p.x) &&
            is_within(r.get_range_y(), p.y) ;
    }

    bool
    operator==(const Recti& lhs, const Recti& rhs)
    {
        return lhs.left == rhs.left
            && lhs.right == rhs.right
            && lhs.top == rhs.top
            && lhs.bottom == rhs.bottom
            ;
    }

    std::ostream&
    operator<<(std::ostream& s, const Recti& r)
    {
        s
            << "("
            << r.left << ", " << r.bottom << " / "
            << r.get_width() << " x " << r.get_height()
            << ")"
            ;
        return s;
    }






    vec2f
    get_random_point(Random* random, const Rectf& r)
    {
        const auto x = get_random_in_range(random, r.get_width());
        const auto y = get_random_in_range(random, r.get_height());
        return r.get_position_from_bottom_left(vec2f{x, y});
    }


    vec2i get_random_point(Random* random, const Recti& r)
    {
        const auto x = get_random_in_range(random, r.get_width());
        const auto y = get_random_in_range(random, r.get_height());
        return r.get_position_from_bottom_left(vec2i{x, y});
    }
}
