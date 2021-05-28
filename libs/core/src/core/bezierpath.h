#pragma once

#include <vector>
#include "core/vec2.h"

namespace euphoria::core
{
    // todo(Gustav): a poly is a closed path, so rename to path
    // todo(Gustav): add more features
    // sebastian lague: bezier support https://github.com/SebLague/Path-Creator
    // paper.js, smooth and simpify(path fitter) path functions: https://github.com/paperjs/paper.js/blob/develop/src/path/PathFitter.js https://github.com/paperjs/paper.js/blob/develop/src/path/PathItem.js#L466
    // path fitter original code: https://github.com/erich666/GraphicsGems/blob/master/gems/FitCurves.c

    // suggested new names:
    // bezier path - a bezier path that can either be open or closed, has anchor points, control points
    // vertex path (or poly line) - a path with points with straight linear interpolated lines, can either be open or closed
    // both paths can either be concave or convex
    //
    // can use a tesselator function to create triangulated/convex polygons from closed paths with closed paths as holes:
    // https://github.com/memononen/libtess2
    // https://github.com/SebLague/Shape-Editor-Tool/blob/master/Shape%20Editor%20E04/Assets/Geometry/Triangulator.cs
    // -> https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf

    // check if point in poly: http://geomalgorithms.com/a03-_inclusion.html

    struct bezier_seg2
    {
        vec2f a0;
        vec2f c0;
        vec2f a1;
        vec2f c1;
    };

    struct bezier_path2
    {
        std::vector<vec2f> points;
        bool is_closed_ = false;

        // todo(Gustav): move out to a controller?
        bool autoset_ = false;

        bezier_path2(const vec2f& center);

        [[nodiscard]] static bool
        is_anchor_point(size_t i);

        [[nodiscard]] static bool
        is_control_point(size_t i);

        // point functions
        void
        add_point(const vec2f& p);
        void
        move_point(int i, const vec2f& delta);

        // segment functions
        [[nodiscard]] size_t
        get_number_of_segments() const;
        [[nodiscard]] bezier_seg2
        get_points_in_segment(size_t i) const;

        void
        set_closed(bool is_closed);
        void
        toggle_closed();

        void
        set_auto_set_control_points(bool is_autoset);
        void
        toggle_auto_set_control_points();

        // private stuff
        [[nodiscard]] size_t
        loop_index(int i) const;
        void
        auto_set_affected_control_points(int updated_anchor_index);
        void
        auto_set_all_control_points();
        void
        auto_set_start_and_end_control_points();
        void
        auto_set_anchor_control_points(int anchor_index);
    };

    // todo(Gustav): merge with svg::Poly
    struct vertex_path2
    {
        std::vector<vec2f> points;
        bool is_closed = false;
    };

}
