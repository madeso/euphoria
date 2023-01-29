#pragma once



#include "core/vec2.h"
#include "core/iterate.h"

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


    // anchor and control points
    // curve passes through anchor points and control points are used for shaping the curve
    // each point in the curve has 2 control points, except the first and last that only has one
    
    struct BezierSegment2
    {
        vec2f a0;
        vec2f c0;

        vec2f a1;
        vec2f c1;
    };

    /*
    Composite Bézier curve or polybezier
    */
    struct PolyBezier2
    {
        std::vector<vec2f> points;
        bool is_closed = false;

        // todo(Gustav): move out to a controller?
        bool is_autoset_enabled = false;

        PolyBezier2(const vec2f& center);

        [[nodiscard]] static bool
        is_anchor_point(size_t i);

        [[nodiscard]] static bool
        is_control_point(size_t i);

        [[nodiscard]] StepIteratorCreator<size_t>
        iterate_points() const;

        void
        add_point(const vec2f& p);

        void
        move_point(int i, const vec2f& delta);

        [[nodiscard]] StepIteratorCreator<int>
        iterate_segments() const;

        [[nodiscard]] BezierSegment2
        get_segment(int i) const;

        void
        set_closed(bool is_closed);

        void
        toggle_closed();

        void
        set_auto_set_control_points(bool is_autoset);

        void
        toggle_auto_set_control_points();
    };


    /*
    wikipedia call this:
    * polygonal chain
    * polygonal curve
    * polygonal path
    * polyline
    * piecewise linear curve
    * broken line
    in geographic information systems:
    * linestring
    * linear ring
    */
    struct Polyline2
    {
        std::vector<vec2f> points;
        bool is_closed = false;
    };

}
