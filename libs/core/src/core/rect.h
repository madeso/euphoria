#pragma once

#include "assert/assert.h"
#include "core/vec2.h"
#include "core/size2.h"
#include "core/range.h"

// Bottom, Left of screen is (0,0)
// X-axis is positive right, Y-axis is positive up

// todo(Gustav): look into better names/easier discoverability for functions

namespace euphoria::core
{
    struct Random;

    struct Rectf;
    struct Recti;

    struct Rectf
    {
        float left;
        float right;
        float top;
        float bottom;

        Rectf();

    private:
        Rectf(float left_side, float right_side, float top_side, float bottom_side);

    public:
        Recti toi() const;

        [[nodiscard]] static Rectf
        from_left_right_bottom_top(float left_side, float right_side, float bottom_side, float top_side);

        [[nodiscard]] static Rectf
        from_left_right_top_bottom(float left_side, float right_side, float top_side, float bottom_side);

        [[nodiscard]] static Rectf
        from_position_anchor_width_and_height
        (
            const vec2f& pos,
            const Scale2<float>& anchor,
            float width,
            float height
        );

        [[nodiscard]]
        static
        Rectf
        from_bottom_left_width_height(const vec2f& bl, float width, float height);

        [[nodiscard]] static Rectf
        from_top_left_width_height(const vec2f& topleft, float width, float height);

        [[nodiscard]] static Rectf
        from_width_height(float width, float height);

        [[nodiscard]] static Rectf
        from_width_height(const size2f& s);

        [[nodiscard]] static Rectf
        from_point(const vec2f& point);

        vec2f
        get_bottom_left() const;

        // centers this rectangle inside the other rectangle and returns it without
        // modifying this
        Rectf center_inside_other(const Rectf& other) const ;

        vec2f get_position_from_bottom_left(const vec2f v) const ;

        float get_relative_center_x_from_bottom_left() const ;

        float get_relative_center_y_from_bottom_left() const ;

        vec2f
        get_relative_center_pos_from_bottom_left() const
        ;

        float
        get_absolute_center_x() const
        ;

        float
        get_absolute_center_y() const
        ;

        vec2f
        get_absolute_center_pos() const
        ;

        // does this contains the argument?
        bool
        contains_exclusive(const Rectf& r) const
        ;

        // on the border is NOT considered included
        bool
        contains_exclusive(const vec2f& p) const
        ;

        bool
        contains_exclusive(float x, float y) const
        ;

        // on the border is considered included
        bool
        contains_inclusive(const vec2f& p) const
        ;

        bool
        contains_inclusive(float x, float y) const
        ;

        Rectf
        get_scaled_around_center_copy(float scale) const
        ;

        void
        scale(float dx, float dy)
        ;

        Rectf
        scale_copy(float dx, float dy) const
        ;

        void
        inset(float dx, float dy)
        ;

        void
        inset(float l, float r, float t, float b)
        ;

        Rectf
        inset_copy(float dx, float dy) const
        ;

        Rectf
        inset_copy(float l, float r, float t, float b) const
        ;

        void
        extend(float dx, float dy)
        ;

        Rectf
        extend_copy(float dx, float dy) const
        ;

        Rectf
        extend_copy(float d) const
        ;

        void
        include(const Rectf& o)
        ;

        // Returns true if the rectangle is empty (left >= right or top <= bottom)
        [[nodiscard]] bool
        is_empty() const
        ;

        // does this represent a rectangle? A 0 width/height is also considered valid
        [[nodiscard]] bool
        is_valid() const
        ;

        // Translate
        void
        offset(float dx, float dy)
        ;

        void
        expand(float expand)
        ;

        Rectf
        expand_copy(float expand)
        ;

        Rectf
        offset_copy(float dx, float dy) const
        ;

        Rectf
        offset_copy(const vec2f& d) const
        ;

        void
        offset_to(float new_left, float new_top)
        ;

        Rectf
        set_top_left_to_copy(float new_left, float new_top) const
        ;

        Rectf
        set_top_left_to_copy(const vec2f& v) const;

        Rectf
        set_bottom_left_to_copy(float new_left, float new_bottom) const
        ;

        Rectf
        set_bottom_left_to_copy(const vec2f& v) const
        ;

        void
        set_empty()
        ;

        // todo(Gustav): add union and intersection functions

        float
        get_height() const
        ;

        float
        get_width() const
        ;

        // todo(Gustav): provide a keep_within, WrapWithin functions
        // like is_within below

        Range<float>
        get_range_y() const
        ;

        Range<float>
        get_range_x() const
        ;

        size2f
        get_size() const
        ;

        [[nodiscard]] vec2f
        get_top_left() const
        ;

        [[nodiscard]] vec2f
        get_top_right() const
        ;

        [[nodiscard]] vec2f
        get_bottom_right() const
        ;

        bool operator==(const Rectf& rhs) = delete;
    };



    struct Recti
    {
        int left;
        int right;
        int top;
        int bottom;

        Recti();

    private:
        Recti(int left_side, int right_side, int top_side, int bottom_side);

    public:
        Rectf tof() const
        ;

        [[nodiscard]] static Recti
        from_left_right_bottom_top(int left_side, int right_side, int bottom_side, int top_side)
        ;

        [[nodiscard]] static Recti
        from_left_right_top_bottom(int left_side, int right_side, int top_side, int bottom_side)
        ;

        [[nodiscard]] static Recti
        from_position_anchor_width_and_height
        (
            const vec2i& pos,
            const Scale2<int>& anchor,
            int width,
            int height
        )
        ;

        [[nodiscard]]
        static
        Recti
        from_bottom_left_width_height(const vec2i& bl, int width, int height)
        ;

        [[nodiscard]] static Recti
        from_top_left_width_height(const vec2i& topleft, int width, int height)
        ;

        [[nodiscard]] static Recti
        from_width_height(int width, int height)
        ;

        [[nodiscard]] static Recti
        from_width_height(const size2i& s)
        ;

        [[nodiscard]] static Recti
        from_point(const vec2i& point)
        ;

        vec2i
        get_bottom_left() const
        ;

        // centers this rectangle inside the other rectangle and returns it without
        // modifying this
        Recti
        center_inside_other(const Recti& other) const
        ;

        vec2i
        get_position_from_bottom_left(const vec2i v) const
        ;

        int
        get_relative_center_x_from_bottom_left() const
        ;

        int
        get_relative_center_y_from_bottom_left() const
        ;

        vec2i
        get_relative_center_pos_from_bottom_left() const
        ;

        int
        get_absolute_center_x() const
        ;

        int
        get_absolute_center_y() const
        ;

        vec2i
        get_absolute_center_pos() const
        ;

        // does this contains the argument?
        bool
        contains_exclusive(const Recti& r) const
        ;

        // on the border is NOT considered included
        bool
        contains_exclusive(const vec2i& p) const
        ;

        bool
        contains_exclusive(int x, int y) const
        ;

        // on the border is considered included
        bool
        contains_inclusive(const vec2i& p) const
        ;

        bool
        contains_inclusive(int x, int y) const
        ;

        Recti
        get_scaled_around_center_copy(int scale) const
        ;

        void
        scale(int dx, int dy)
        ;

        Recti
        scale_copy(int dx, int dy) const
        ;

        void
        inset(int dx, int dy)
        ;

        void
        inset(int l, int r, int t, int b)
        ;

        Recti
        inset_copy(int dx, int dy) const
        ;

        Recti
        inset_copy(int l, int r, int t, int b) const
        ;

        void
        extend(int dx, int dy)
        ;

        Recti
        extend_copy(int dx, int dy) const
        ;

        Recti
        extend_copy(int d) const
        ;

        void
        include(const Recti& o)
        ;

        // Returns true if the rectangle is empty (left >= right or top <= bottom)
        [[nodiscard]] bool
        is_empty() const
        ;

        // does this represent a rectangle? A 0 width/height is also considered valid
        [[nodiscard]] bool
        is_valid() const
        ;

        // Translate
        void
        offset(int dx, int dy)
        ;

        void
        expand(int expand)
        ;

        Recti
        expand_copy(int expand)
        ;

        Recti
        offset_copy(int dx, int dy) const
        ;

        Recti
        offset_copy(const vec2i& d) const
        ;

        void
        offset_to(int new_left, int new_top)
        ;

        Recti
        set_top_left_to_copy(int new_left, int new_top) const
        ;

        Recti
        set_top_left_to_copy(const vec2i& v) const;

        Recti
        set_bottom_left_to_copy(int new_left, int new_bottom) const
        ;

        Recti
        set_bottom_left_to_copy(const vec2i& v) const
        ;

        void
        set_empty()
        ;

        // todo(Gustav): add union and intersection functions

        int
        get_height() const
        ;

        int
        get_width() const
        ;

        // todo(Gustav): provide a keep_within, WrapWithin functions
        // like is_within below

        Range<int>
        get_range_y() const
        ;

        Range<int>
        get_range_x() const
        ;

        size2i
        get_size() const
        ;

        [[nodiscard]] vec2i
        get_top_left() const
        ;

        [[nodiscard]] vec2i
        get_top_right() const
        ;

        [[nodiscard]] vec2i
        get_bottom_right() const
        ;
    };


    [[nodiscard]] vec2f
    to01(const Rectf& r, const vec2f& from)
    ;

    [[nodiscard]] vec2f
    from01(const Rectf& r, const vec2f& from)
    ;

    bool
    is_within(const Rectf& r, const vec2f& p)
    ;

    std::ostream&
    operator<<(std::ostream& s, const Rectf& r)
    ;

    [[nodiscard]] vec2f
    to01(const Recti& r, const vec2i& from)
    ;

    [[nodiscard]] vec2i
    from01(const Recti& r, const vec2f& from)
    ;

    bool
    is_within(const Recti& r, const vec2i& p)
    ;

    bool
    operator==(const Recti& lhs, const Recti& rhs)
    ;

    std::ostream&
    operator<<(std::ostream& s, const Recti& r)
    ;

    vec2f get_random_point(Random* random, const Rectf& r);
    vec2i get_random_point(Random* random, const Recti& r);
}
