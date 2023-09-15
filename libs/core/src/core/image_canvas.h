#pragma once



#include "core/rgb.h"
#include "core/vec2.h"
#include "core/mat3.h"

namespace eu::core
{
    struct Image;

    /// hacky layer between something that looks like the html 'canvas rendering context 2d' and the euphoria image drawing operations
    /// https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D
    struct Canvas
    {
        Rgbi fill_style;
        Image* target_image;
        mat3f transform;
        bool building_path;
        std::vector<vec2f> path;

        Canvas(Image* i);

        void translate(float x, float y);
        void rotate(float r);
        void begin_path();
        void close_path();
        void move_to(float x, float y);

        /// should be named line_to but that name is horrible
        void draw_line_to(float dx, float dy);

        [[nodiscard]] vec2f transform_position(const vec2f& v) const;
        void fill_rect(int x, int y, int w, int h) const;

        void fill() const;
    };
}

