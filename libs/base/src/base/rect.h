#pragma once

#include "assert/assert.h"

#include "base/vec2.h"
#include "base/range.h"

// Bottom, Left of screen is (0,0)
// X-axis is positive right, Y-axis is positive up


namespace eu
{
    enum class RectCut { left, right, top, bottom };

    /// offset from sides
    /// \note unlike css this follows the (left right) (top bottom) order
    struct Lrtb
    {
        float left; float right; float top; float bottom;
        constexpr explicit Lrtb(float all) : left(all), right(all), top(all), bottom(all) {}
        constexpr Lrtb(float lr, float tb) : left(lr), right(lr), top(tb), bottom(tb) {}
        constexpr Lrtb(float l, float r, float t, float b) : left(l), right(r), top(t), bottom(b) {}
    };

    struct Rect
    {
        float left; // min x
        float right; // max x
        float top; // max y
        float bottom; // min y

        Rect();

        bool operator==(const Rect& rhs) = delete;
    

        [[nodiscard]] static constexpr Rect from_left_right_bottom_top(float left_side, float right_side, float bottom_side, float top_side)
        {
            ASSERTX(left_side <= right_side, left_side, right_side);
            ASSERTX(top_side >= bottom_side, top_side, bottom_side);
            return { left_side, right_side, top_side, bottom_side };
        }

        [[nodiscard]] static Rect from_left_right_top_bottom(float left_side, float right_side, float top_side, float bottom_side);
        [[nodiscard]] static Rect from_bottom_left_size(const v2& bl, const v2& size);
        [[nodiscard]] static Rect from_top_left_size(const v2& top_left, const v2& size);
        [[nodiscard]] static Rect from_size(const v2& size);
        [[nodiscard]] static Rect from_point(const v2& point);

        // extend/include/union current rect with another point/rect
        void extend(const Rect& o);

        void translate(const v2& v);
        void inset(const Lrtb& lrtb); // moves each side towards the center

        void set_empty();

        /// centers the self rectangle inside the other rectangle
        [[nodiscard]] static Rect from_center_inside_other(const Rect& self, const Rect& other);

        /// scaled copy around rect center
        [[nodiscard]] static Rect from_scaled(const Rect& self, float scale);

        /// does this contains the argument?
        [[nodiscard]] bool contains_exclusive(const Rect& r) const;

        [[nodiscard]] Rect with_inset(const Lrtb& lrtb) const;
        [[nodiscard]] Rect with_translate(const v2& v) const;
        [[nodiscard]] Rect with_top_left_at(const v2& p) const;
        [[nodiscard]] Rect with_bottom_left_at(const v2& p) const;
        [[nodiscard]] Rect with_offset(const v2& p) const;
        [[nodiscard]] Rect with_scale(float hor, float vert) const;

        [[nodiscard]] v2 get_bottom_left() const;
        [[nodiscard]] v2 get_center_pos() const;

        // position is from bottom left
        [[nodiscard]] v2 get_relative_center_pos() const;
        
        // Returns true if the rectangle is empty (left >= right or top <= bottom)
        [[nodiscard]] bool is_empty() const;

        // a 0 width/height is also considered valid
        [[nodiscard]] bool is_valid() const;

        [[nodiscard]] v2 get_size() const;

        [[nodiscard]] R<float> get_range_y() const;
        [[nodiscard]] R<float> get_range_x() const;
        [[nodiscard]] v2 get_top_left() const;
        [[nodiscard]] v2 get_top_right() const;
        [[nodiscard]] v2 get_bottom_right() const;

        Rect cut_left(float amount, float h_spacing = 0.0f);
        Rect cut_right(float amount, float h_spacing = 0.0f);
        Rect cut_bottom(float amount, float v_spacing = 0.0f);
        Rect cut_top(float amount, float v_spacing = 0.0f);

        Rect cut(RectCut side, float amount, float spacing = 0.0f);

        // Same as cut, except they keep the input rect intact.
        // Useful for decorations
        Rect get_left(float amount) const;
        Rect get_right(float amount) const;
        Rect get_bottom(float amount) const;
        Rect get_top(float amount) const;

        // These will add a rectangle outside of the input rectangle.
        // Useful for tooltips and other overlay elements.
        Rect add_left(float amount) const;
        Rect add_right(float amount) const;
        Rect add_bottom(float amount) const;
        Rect add_top(float amount) const;

    private:
        constexpr Rect(float left_side, float right_side, float top_side, float bottom_side)
            : left(left_side)
            , right(right_side)
            , top(top_side)
            , bottom(bottom_side)
        {
        }
    };

    [[nodiscard]] v2 to_01(const Rect& r, const v2& from);
    [[nodiscard]] v2 from_01(const Rect& r, const v2& from);

    /// is the point contained within the rect?
    /// \note on the border is considered within
    bool is_within(const v2& p, const Rect& r);

    std::string to_string(const Rect& r);
}

ADD_DEFAULT_FORMATTER(eu::Rect, std::string, eu::to_string);
