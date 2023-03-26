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

        bool operator==(const Rectf& rhs) = delete;
    

        [[nodiscard]] static Rectf from_left_right_bottom_top(float left_side, float right_side, float bottom_side, float top_side);
        [[nodiscard]] static Rectf from_left_right_top_bottom(float left_side, float right_side, float top_side, float bottom_side);
        [[nodiscard]] static Rectf from_position_anchor_width_and_height
        (
            const vec2f& pos,
            const Scale2f& anchor,
            float width,
            float height
        );
        [[nodiscard]] static Rectf from_bottom_left_width_height(const vec2f& bl, float width, float height);
        [[nodiscard]] static Rectf from_top_left_width_height(const vec2f& topleft, float width, float height);
        [[nodiscard]] static Rectf from_width_height(float width, float height);
        [[nodiscard]] static Rectf from_width_height(const size2f& s);
        [[nodiscard]] static Rectf from_point(const vec2f& point);


        // offset = translate
        void offset(float dx, float dy);
        void offset_to(float new_left, float new_top);
        void inset(float dx, float dy);
        void inset(float l, float r, float t, float b);
        void extend(float dx, float dy);
        void include(const Rectf& o);
        void expand(float expand);
        void scale(float dx, float dy);
        void set_empty();


        // centers this rectangle inside the other rectangle and returns it without
        // modifying this
        [[nodiscard]] Rectf center_inside_other(const Rectf& other) const ;
        [[nodiscard]] vec2f get_position_from_bottom_left(const vec2f& v) const ;
        // does this contains the argument?
        [[nodiscard]] bool contains_exclusive(const Rectf& r) const;

        // on the border is NOT considered included
        [[nodiscard]] bool contains_exclusive(const vec2f& p) const;
        [[nodiscard]] bool contains_exclusive(float x, float y) const;

        // on the border is considered included
        [[nodiscard]] bool contains_inclusive(const vec2f& p) const;
        [[nodiscard]] bool contains_inclusive(float x, float y) const;
        [[nodiscard]] Rectf get_scaled_around_center_copy(float scale) const;
        [[nodiscard]] Rectf scale_copy(float dx, float dy) const;
        [[nodiscard]] Rectf inset_copy(float dx, float dy) const;
        [[nodiscard]] Rectf inset_copy(float l, float r, float t, float b) const;
        [[nodiscard]] Rectf extend_copy(float dx, float dy) const;
        [[nodiscard]] Rectf extend_copy(float d) const;
        [[nodiscard]] Rectf expand_copy(float expand) const;
        [[nodiscard]] Rectf offset_copy(float dx, float dy) const;
        [[nodiscard]] Rectf offset_copy(const vec2f& d) const;
        [[nodiscard]] Rectf set_top_left_to_copy(float new_left, float new_top) const;
        [[nodiscard]] Rectf set_top_left_to_copy(const vec2f& v) const;
        [[nodiscard]] Rectf set_bottom_left_to_copy(float new_left, float new_bottom) const;
        [[nodiscard]] Rectf set_bottom_left_to_copy(const vec2f& v) const;


        [[nodiscard]] Recti to_i() const;
        [[nodiscard]] vec2f get_bottom_left() const;
        [[nodiscard]] float get_relative_center_x_from_bottom_left() const ;
        [[nodiscard]] float get_relative_center_y_from_bottom_left() const ;
        [[nodiscard]] vec2f get_relative_center_pos_from_bottom_left() const;
        [[nodiscard]] float get_absolute_center_x() const;
        [[nodiscard]] float get_absolute_center_y() const;
        [[nodiscard]] vec2f get_absolute_center_pos() const;
        // Returns true if the rectangle is empty (left >= right or top <= bottom)
        [[nodiscard]] bool is_empty() const;

        // does this represent a rectangle? A 0 width/height is also considered valid
        [[nodiscard]] bool is_valid() const;

        [[nodiscard]] float get_height() const;
        [[nodiscard]] float get_width() const;

        [[nodiscard]] Range<float> get_range_y() const;
        [[nodiscard]] Range<float> get_range_x() const;
        [[nodiscard]] size2f get_size() const;
        [[nodiscard]] vec2f get_top_left() const;
        [[nodiscard]] vec2f get_top_right() const;
        [[nodiscard]] vec2f get_bottom_right() const;

    private:
        Rectf(float left_side, float right_side, float top_side, float bottom_side);
    };



    struct Recti
    {
        int left;
        int right;
        int top;
        int bottom;

        Recti();


        [[nodiscard]] static Recti from_position_anchor_width_and_height
        (
            const vec2i& pos,
            const Scale2f& anchor,
            int width,
            int height
        );

        [[nodiscard]] static Recti from_left_right_bottom_top(int left_side, int right_side, int bottom_side, int top_side);
        [[nodiscard]] static Recti from_left_right_top_bottom(int left_side, int right_side, int top_side, int bottom_side);
        [[nodiscard]] static Recti from_bottom_left_width_height(const vec2i& bl, int width, int height);
        [[nodiscard]] static Recti from_top_left_width_height(const vec2i& topleft, int width, int height);
        [[nodiscard]] static Recti from_width_height(int width, int height);
        [[nodiscard]] static Recti from_width_height(const size2i& s);
        [[nodiscard]] static Recti from_point(const vec2i& point);

        // offset = translate
        void offset(int dx, int dy);
        void scale(int dx, int dy);
        void inset(int dx, int dy);
        void inset(int l, int r, int t, int b);
        void expand(int expand);
        void include(const Recti& o);
        void offset_to(int new_left, int new_top);
        void extend(int dx, int dy);
        void set_empty();

        // centers this rectangle inside the other rectangle and returns it without
        // modifying this
        [[nodiscard]] Recti center_inside_other(const Recti& other) const;
        [[nodiscard]] vec2i get_position_from_bottom_left(const vec2i& v) const;
        
        // does this contains the argument?
        [[nodiscard]] bool contains_exclusive(const Recti& r) const;

        // on the border is NOT considered included
        [[nodiscard]] bool contains_exclusive(const vec2i& p) const;
        [[nodiscard]] bool contains_exclusive(int x, int y) const;

        // on the border is considered included
        [[nodiscard]] bool contains_inclusive(const vec2i& p) const;
        [[nodiscard]] bool contains_inclusive(int x, int y) const;
        [[nodiscard]] Recti get_scaled_around_center_copy(int scale) const;

        [[nodiscard]] Recti expand_copy(int expand) const;
        [[nodiscard]] Recti offset_copy(int dx, int dy) const;
        [[nodiscard]] Recti offset_copy(const vec2i& d) const;
        [[nodiscard]] Recti extend_copy(int dx, int dy) const;
        [[nodiscard]] Recti extend_copy(int d) const;
        [[nodiscard]] Recti scale_copy(int dx, int dy) const;
        [[nodiscard]] Recti inset_copy(int dx, int dy) const;
        [[nodiscard]] Recti inset_copy(int l, int r, int t, int b) const;

        [[nodiscard]] Recti set_top_left_to_copy(int new_left, int new_top) const;
        [[nodiscard]] Recti set_top_left_to_copy(const vec2i& v) const;
        [[nodiscard]] Recti set_bottom_left_to_copy(int new_left, int new_bottom) const;
        [[nodiscard]] Recti set_bottom_left_to_copy(const vec2i& v) const;

        [[nodiscard]] Rectf to_f() const;
        [[nodiscard]] int get_relative_center_x_from_bottom_left() const;
        [[nodiscard]] int get_relative_center_y_from_bottom_left() const;
        [[nodiscard]] vec2i get_relative_center_pos_from_bottom_left() const;
        [[nodiscard]] int get_absolute_center_x() const;
        [[nodiscard]] int get_absolute_center_y() const;
        [[nodiscard]] vec2i get_absolute_center_pos() const;

        // Returns true if the rectangle is empty (left >= right or top <= bottom)
        [[nodiscard]] bool is_empty() const;

        // does this represent a rectangle? A 0 width/height is also considered valid
        [[nodiscard]] bool is_valid() const;

        [[nodiscard]] vec2i get_top_left() const;
        [[nodiscard]] vec2i get_top_right() const;
        [[nodiscard]] vec2i get_bottom_left() const;
        [[nodiscard]] vec2i get_bottom_right() const;

        [[nodiscard]] int get_height() const;
        [[nodiscard]] int get_width() const;
        [[nodiscard]] size2i get_size() const;

        [[nodiscard]] Range<int> get_range_y() const;
        [[nodiscard]] Range<int> get_range_x() const;


    private:
        Recti(int left_side, int right_side, int top_side, int bottom_side);
    };


    [[nodiscard]] vec2f to01(const Rectf& r, const vec2f& from);
    [[nodiscard]] vec2f to01(const Recti& r, const vec2i& from);
    [[nodiscard]] vec2f from_01(const Rectf& r, const vec2f& from);
    [[nodiscard]] vec2i from_01(const Recti& r, const vec2f& from);

    // todo(Gustav): add union and intersection functions

    // todo(Gustav): provide a keep_within, WrapWithin functions
    // like is_within below
    bool is_within(const Rectf& r, const vec2f& p);
    bool is_within(const Recti& r, const vec2i& p);

    vec2f get_random_point(Random* random, const Rectf& r);
    vec2i get_random_point(Random* random, const Recti& r);

    std::string to_string(const Rectf& r);
    std::string to_string(const Recti& r);

    bool operator==(const Recti& lhs, const Recti& rhs);
}

ADD_DEFAULT_FORMATTER(euphoria::core::Rectf, std::string, euphoria::core::to_string);
ADD_DEFAULT_FORMATTER(euphoria::core::Recti, std::string, euphoria::core::to_string);
