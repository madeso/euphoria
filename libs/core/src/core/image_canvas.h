#ifndef EUPHORIA_CORE_IMAGE_CANVAS_H
#define EUPHORIA_CORE_IMAGE_CANVAS_H

#include <vector>

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
        Rgbi fillStyle;

        Image* image;

        mat3f transform;

        bool building_path;
        std::vector<vec2f> path;

        [[nodiscard]] vec2f
        C(const vec2f& v) const;

        Canvas(Image* i);

        void
        fillRect(int x, int y, int w, int h) const;

        void
        translate(float x, float y);

        void
        rotate(float r);

        void
        beginPath();

        void
        closePath();

        void
        moveTo(float x, float y);

        void
        lineTo(float dx, float dy);

        void
        fill() const;
    };
}

#endif  // EUPHORIA_CORE_IMAGE_CANVAS_H

