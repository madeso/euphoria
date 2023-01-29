#pragma once



#include "core/rgb.h"
#include "core/vec2.h"
#include "core/mat3.h"

namespace euphoria::core
{
    struct Image;

    // hacky layer between something that looks like the html 'canvas rendering context 2d' and the euphoria image drawing operations
    // https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D
    struct Canvas
    {
        rgbi fill_style;

        Image* target_image;

        mat3f transform;

        bool building_path;
        std::vector<vec2f> path;

        [[nodiscard]] vec2f
        transform_position(const vec2f& v) const;

        Canvas(Image* i);

        void
        fill_rect(int x, int y, int w, int h) const;

        void
        translate(float x, float y);

        void
        rotate(float r);

        void
        begin_path();

        void
        close_path();

        void
        move_to(float x, float y);

        void
        line_to(float dx, float dy);

        void
        fill() const;
    };
}

